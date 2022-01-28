#include <fstream>

#include "bpcommon.hpp"
#include "Bp.hpp"
#include "BpModLib.hpp"
#include "BpNodeLib.hpp"
#include "BpModLibLinux.hpp"
#include "BpGraphModLib.hpp"
#include "BpNodeVar.hpp"
#include "BpGraph.hpp"

namespace bp {
	
Bp& Bp::Instance() {
	static Bp instance;
	return instance;
}

Bp::Bp() 
	: _version({0, 0, 0})
{
	_cur_edit_graph.reset();

	_base_mods = std::make_shared<BpModLibLinux>();
	_base_mods->Init("../conf/");
	_graph_mods = std::make_shared<BpGraphModLib>();
	_graph_mods->Init("/home/caros/mod/");
	_nodes_lib = std::make_shared<BpNodeLib>();

	_contents = std::make_shared<BpContents>(nullptr, "", BpContents::Type::CONTENTS);
	// 事件节点
	auto& node_contents = _nodes_lib->GetContents()->GetChildren();
	for (int i = 0; i < node_contents.size(); ++i) {
		_contents->AddChild(node_contents[i]);
	}
	// 函数/变量节点
	auto& mods = _base_mods->GetMods();
	for (int i = 0; i < mods.size(); ++i) {
		_contents->AddChild(mods[i]->GetContents());
	}
	// 组织模块节点
	_contents->AddChild(_graph_mods->GetContents());
}

Bp::~Bp() {
	_edit_graphs.clear();
}

std::vector<int> Bp::Version(const std::string& path) {
	std::ifstream ifs(path);
	if (!ifs.is_open()) {
		LOG(ERROR) << "Open version file \"" << path << "\" failed";
		return std::vector<int>({0, 0, 0});
	}
    std::stringstream ss;
	ss << ifs.rdbuf();
    std::string num_str;
	std::vector<int> res;
    while(std::getline(ss, num_str, '.')) {
        try {
            res.push_back(std::stoi(num_str));
        } catch (std::exception &e) {
            LOG(ERROR) << "Get version \"" << num_str << "\" failed";
            return std::vector<int>({0, 0, 0});
        }
    }
    return res;
}

bool Bp::RegisterUserMod(std::shared_ptr<BpContents> contents, std::function<std::shared_ptr<BpNode>(const std::string&)> func) {
	auto user_mode_name = contents->GetName();
	if (_user_spawn_nodes.find(user_mode_name) != _user_spawn_nodes.end()) {
		LOG(ERROR) << "Register user node " << user_mode_name;
		return false;
	}
	_user_spawn_nodes[user_mode_name] = func;
	_contents->AddChild(contents);
	return true;
}

BpVariable Bp::CreateVariable(const std::string& type, const std::string& name) {
	return CreateVariable(type, name, "");
}

BpVariable Bp::CreateVariable(const std::string& type, const std::string& name, const std::string& value_desc) {
	auto v = _base_mods->CreateVal(type);
	auto var = BpVariable(type, name, v);
	if (v == nullptr) {
		LOG(ERROR) << "Create var " << type << " " << name << " = " << value_desc << " failed";
		return var;
	}
	if (!value_desc.empty()) {
		var.SetValue(value_desc);
	}
	return var;
}

LoadSaveState Bp::LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g, Json::Value& desc) {
	std::ifstream ifs(bp_json_path);
	if (!ifs.is_open()) {
		LOG(ERROR) << "Open \"" << bp_json_path << "\" failed";
		return LoadSaveState::ERR_OPEN_FILE;
	}
	Json::Value root;
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(ifs, root)) {
		LOG(ERROR) << "Parse \"" << bp_json_path << "\" failed";
        return LoadSaveState::ERR_PARSE;
    }
	LOG(INFO) << "Load \"" << bp_json_path << "\"...";
	std::string graph_name = "";
	Json::Value graph_json;
	// FIXME: if has __main__ do or other
	Json::Value::Members mem = root.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); ++iter) {
		graph_name = *iter;
		break;
	}
	if (graph_name.empty()) {
		LOG(ERROR) << "can't find graph name";
		return LoadSaveState::ERR_JSON_FMT;
	}
	if (graph_name == "__main__") {
		g = std::make_shared<BpGraph>("__main__", BpNodeType::BP_GRAPH_EXEC, nullptr);
	} else {
		g = std::make_shared<BpGraph>(graph_name, BpNodeType::BP_GRAPH, nullptr);;
		g->AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
		g->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
	}
	return LoadGraph(root, graph_name, g, desc);
}

LoadSaveState Bp::LoadGraph(const Json::Value& root, const std::string& graph_name, std::shared_ptr<BpGraph>& g, Json::Value& desc) {
	Json::Value graph = root[graph_name];
	return LoadGraph(root, graph, g, desc);
}

LoadSaveState Bp::LoadGraph(const Json::Value& root, const Json::Value& json_graph, std::shared_ptr<BpGraph>& g, Json::Value& desc) {
	g->_name = json_graph["name"].asString();
	// variables
	auto vars = json_graph["vars"];
	for (int i = 0; i < vars.size(); ++i) {
		auto type = vars[i]["type"].asString();
		auto name = vars[i]["name"].asString();
		auto get = vars[i]["get"].asBool();
		auto value_desc = vars[i]["value"].asString();
		auto var = CreateVariable(type, name, value_desc);
		if (var.IsNone()) {
			LOG(ERROR) << "create var " << name << " failed";
			return LoadSaveState::ERR_VAR;
		}
		g->AddVariable(name, var);
	}
	// nodes
	auto nodes = json_graph["nodes"];
	Json::Value::Members mem = nodes.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); ++iter) {
		int id = std::stoi(*iter);
		auto json_node = nodes[*iter];
		// node pos
		if (json_node.isMember("pos")){
			desc[*iter]["pos"] = json_node["pos"];
        }
		// node attr
		BpNodeType t = (BpNodeType)json_node["type"].asInt();
		auto node_name = json_node["name"].asString();
		auto style_name = json_node["style"].asString();
		std::shared_ptr<BpNode> node = nullptr;
		if (t == BpNodeType::BP_NODE_VAR) {
			bool get = json_node["get"].asBool();
			node = SpawnVarNode(g, node_name, get);
		} else if (t == BpNodeType::BP_GRAPH_EXEC) {
			LOG(WARNING) << "Can't load BP_GRAPH_EXEC graph, continue";
			continue;
		} else {
			node = SpawnNode(node_name, t);
		}
		if (node == nullptr) {
			LOG(ERROR) << "Load node " << node_name << " failed";
			return LoadSaveState::ERR_CREATE_NODE;
		}
		g->SetNextID(id);
		if (t == BpNodeType::BP_NODE_EV) {
			g->AddEventNode(node);
		} else {
			g->AddNode(node);
		}
	}
	g->SetNodesPos(desc);
	// 解析link并创建link
	auto links = json_graph["links"];
	for (int i = 0; i < links.size(); ++i) {
		int link_id = links[i]["id"].asInt();
		int start_id = links[i]["start_id"].asInt();
		int end_id = links[i]["end_id"].asInt();
		
		auto sp = g->SearchPin(start_id);
		auto ep = g->SearchPin(end_id);
		if (sp == nullptr || ep == nullptr) {
			LOG(ERROR) << "Search pin " << start_id << " or " << end_id << " failed";
			return LoadSaveState::ERR_SEARCH_PIN;
		}
		g->SetNextID(link_id);
		g->AddLink(*sp, *ep);
		// link color
		std::vector<BpLink>& ls = g->GetLinks();
		auto colors = links[i]["color"];
		for (int j = 0; j < colors.size(); ++j) {
			ls.back().Color[j] = colors[j].asFloat();
		}
	}
	g->SetNextID(json_graph["max_id"].asInt());
	return LoadSaveState::OK;
}

LoadSaveState Bp::SaveGraph(const std::string& bp_json_path, const std::shared_ptr<BpGraph>& g, const Json::Value& desc) {
	std::ofstream ofs(bp_json_path);
	if (!ofs.is_open()) {
		LOG(ERROR) << "Open \"" << bp_json_path << "\" failed";
		return LoadSaveState::ERR_OPEN_FILE;
	}
	Json::Value root;
	auto res = SaveGraph(root, g, desc);
	Json::StyledWriter sw;
	ofs << sw.write(root);
	ofs.close();
	return res;
}

LoadSaveState Bp::SaveGraph(Json::Value& root, const std::shared_ptr<BpGraph>& g, const Json::Value& desc) {
	Json::Value json_graph;
	json_graph["name"] = g->GetName();
	// ev_nodes
	auto& ev_nodes = g->GetEvNodes();
	for (auto& ev : ev_nodes) {
		Json::Value json_evnode;
		json_evnode["name"] = ev.first;
		json_evnode["type"] = (int)ev.second->GetNodeType();
		json_evnode["style"] = (int)ev.second->GetNodeStyle();
		json_evnode["id"] = ev.second->GetID();
		json_graph["nodes"][std::to_string(ev.second->GetID())] = json_evnode;
	}
	// var/base/nor nodes
	auto& nor_nodes = g->GetNodes();
	for (auto& nor : nor_nodes) {
		Json::Value json_nornode;
		auto node_type = nor->GetNodeType();
		json_nornode["name"] = nor->GetName();
		json_nornode["type"] = (int)node_type;
		json_nornode["style"] = (int)nor->GetNodeStyle();
		json_nornode["id"] = nor->GetID();
		if (nor->GetNodeType() == BpNodeType::BP_NODE_VAR) {
			auto var_node = std::dynamic_pointer_cast<BpNodeVar>(nor);
			json_nornode["get"] = var_node->IsGet();
		}
		json_graph["nodes"][std::to_string(nor->GetID())] = json_nornode;
	}
	// nodes pos
	Json::Value::Members mem = desc.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); ++iter) {
		json_graph["nodes"][*iter]["pos"] = desc[*iter]["pos"];
	}
	// links
	auto& links = g->GetLinks();
	for (auto& l : links) {
		Json::Value json_link;
		json_link["id"] = l.ID;
		json_link["start_id"] = l.StartPinID;
		json_link["end_id"] = l.EndPinID;
		Json::Value json_link_color;
		json_link_color.append(l.Color[0]);
		json_link_color.append(l.Color[1]);
		json_link_color.append(l.Color[2]);
		json_link_color.append(l.Color[3]);
		json_link["color"] = json_link_color;
		json_graph["links"].append(json_link);
	}
	// vars
	auto& vars = g->GetVariables();
	for (auto& x : vars) {
		Json::Value json_var;
		json_var["name"] = x.first;
		json_var["type"] = x.second.GetType();
		json_var["value"] = x.second.ToJson();
		json_graph["vars"].append(json_var);
	}
	json_graph["max_id"] = g->GetNextID();
	// graph name
	root[g->GetName()] = json_graph;
	return LoadSaveState::OK;
}

std::shared_ptr<BpNode> Bp::SpawnNode(const std::string& node_name, const BpNodeType t) {
	// 从BpModLib中获得函数指针和描述
	// 从BpNodeLib中获得节点对象
	// 组合成Node添加到graph
	// _base_mods->GetMods()[0]->GetContents();
	if (t == BpNodeType::BP_NODE_FUNC) {
		auto func_info = _base_mods->GetFunc(node_name);
		if (func_info.type == BpModuleFuncType::UNKNOWN) {
			LOG(ERROR) << "get func " << node_name << " failed";
			return nullptr;
		}
		func_info.name = node_name;
		// args init
		std::vector<BpVariable> args;
		for (int i = 0; i < func_info.type_args.size(); ++i) {
			auto var = CreateVariable(func_info.type_args[i], func_info.type_args[i]);
			if (var.IsNone()) {
				LOG(ERROR) << "create var " << func_info.type_args[i] << " failed";
				return nullptr;
			}
			args.emplace_back(var);
		}
		// res init
		std::vector<BpVariable> res;
		for (int i = 0; i < func_info.type_res.size(); ++i) {
			auto var = CreateVariable(func_info.type_res[i], func_info.type_res[i]);
			if (var.IsNone()) {
				LOG(ERROR) << "create var " << func_info.type_res[i] << " failed";
				return nullptr;
			}
			res.emplace_back(var);
		}
		return _nodes_lib->CreateFuncNode(func_info, args, res);
	} else if (t == BpNodeType::BP_NODE_EV) {
		return _nodes_lib->CreateEvNode(node_name);
	} else if (t == BpNodeType::BP_NODE_BASE) {
		return _nodes_lib->CreateBaseNode(node_name);
	} else if (t == BpNodeType::BP_NODE_USER) {
		auto user_mod_name = BpCommon::GetModName(node_name);
		if (_user_spawn_nodes.find(user_mod_name) == _user_spawn_nodes.end()) {
			return nullptr;
		}
		return _user_spawn_nodes[user_mod_name](node_name);
	} else if (t == BpNodeType::BP_GRAPH) {
		auto graph_desc = _graph_mods->GetGraphDesc(node_name);
		auto graph = std::make_shared<BpGraph>(node_name, t, nullptr);;
		graph->AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
		graph->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
		if (!graph_desc.isNull()) {
			LoadSaveState state = LoadSaveState::OK;
			Json::Value nodes_pos = _graph_mods->GetNodesPosFromDesc(graph_desc);
			if (LoadSaveState::OK != (state = LoadGraph(Json::Value(), graph_desc, graph, nodes_pos))) {
				LOG(ERROR) << "load mod graph failed, " << (int)state;
				return nullptr;
			}
		} else {
			graph->AddNode(SpawnNode("input", BpNodeType::BP_GRAPH_INPUT));
			graph->AddNode(SpawnNode("input", BpNodeType::BP_GRAPH_OUTPUT));
			LOG(WARNING) << "mod graph lib has no graph: " << node_name;
		}
		return graph;
	} else if (t == BpNodeType::BP_GRAPH_EXEC) {
		return std::make_shared<BpGraph>("__main__", t, nullptr);
	} else if (t == BpNodeType::BP_GRAPH_INPUT) {
		auto input = std::make_shared<BpNode>("input", nullptr);
		input->SetNodeType(BpNodeType::BP_GRAPH_INPUT);
		input->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
		return input;
	}else if (t == BpNodeType::BP_GRAPH_OUTPUT) {
		auto output = std::make_shared<BpNode>("output", nullptr);
		output->SetNodeType(BpNodeType::BP_GRAPH_OUTPUT);
		return output;
	}
	return nullptr;
}

std::shared_ptr<BpNode> Bp::SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_name, bool is_get) {
	auto var = g->GetVariable(var_name);
	if (var.IsNone()) {
		LOG(ERROR) << "var: " << var_name << "is empty";
		return nullptr;
	}
	return _nodes_lib->CreateVarNode(var, is_get);
}

std::shared_ptr<BpNode> Bp::SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_type, const std::string& var_name, bool is_get) {
	auto msg = _base_mods->CreateVal(var_type);
	if (msg == nullptr) {
		LOG(ERROR) << "create var failed";
		return nullptr;
	}
	g->AddVariable(var_name, BpVariable(var_type, var_name, msg));
	return _nodes_lib->CreateVarNode(g->GetVariable(var_name), is_get);
}

const std::shared_ptr<BpContents>& Bp::GetContents() const {
	return _contents;
}

bool Bp::AddEditGraph(const std::string& name, std::shared_ptr<BpGraph> g) {
	if (_edit_graphs.find(name) != _edit_graphs.end()) {
		LOG(ERROR) << "add edit graph " << name << "failed";
		return false;
	}
	_edit_graphs[name] = g;
	return true;
}

} // nemespace bp
