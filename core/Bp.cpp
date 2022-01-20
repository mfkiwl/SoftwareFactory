#include <fstream>

#include "Bp.hpp"
#include "BpModLib.hpp"
#include "BpNodeLib.hpp"
#include "BpModLibLinux.hpp"
#include "BpBaseNodeVar.hpp"
#include "BpGraph.hpp"

namespace bp {
	
Bp& Bp::Instance() {
	static Bp instance;
	return instance;
}

Bp::Bp() {
	_cur_edit_graph.reset();

	_base_mods = std::make_shared<BpModLibLinux>();
	_base_mods->Init("../conf/");
	_nodes_lib = std::make_shared<BpNodeLib>();

	_contents = std::make_shared<BpContents>(nullptr, "", BpContents::Type::CONTENTS);
	// 事件节点
	auto& node_contents = _nodes_lib->GetContents()->GetChildren();
	for (int i = 0; i < node_contents.size(); ++i) {
		_contents->AddChild(node_contents[i]);
	}
	// 函数变量节点
	auto& mods = _base_mods->GetMods();
	for (int i = 0; i < mods.size(); ++i) {
		_contents->AddChild(mods[i]->GetContents());
	}
}

Bp::~Bp() {
	_edit_graphs.clear();
	_graphs.clear();
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

LoadSaveState Bp::LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g) {
	std::ifstream ifs(bp_json_path);
	if (!ifs.is_open()) {
		LOG(ERROR) << "Open \"" << bp_json_path << "\" failed";
		return LoadSaveState::ERR_OPEN_FILE;
	}
	Json::Value value;
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(ifs, value)) {
		LOG(ERROR) << "Parse \"" << bp_json_path << "\" failed";
        return LoadSaveState::ERR_PARSE;
    }
	LOG(INFO) << "Load \"" << bp_json_path << "\"...";
	return LoadGraph(value, g);
}

LoadSaveState Bp::LoadGraph(const Json::Value& root, std::shared_ptr<BpGraph>& g) {
	// 检查配置文件版本是否小于当前程序版本
	auto bin_ver = bp::Bp::Instance().Version();
	if (bin_ver != root["version"].asString()) {
		LOG(ERROR) << "Version error: bin " << bin_ver << ", conf \""
			<< root["version"].asString();
		return LoadSaveState::ERR_VERSION;
	}

	g->Clear();
	
	// 解析变量并创建变量
	auto vars = root["variables"];
	for (int i = 0; i < vars.size(); ++i) {
		auto type = vars[i]["type"].asString();
		auto name = vars[i]["name"].asString();
		auto value_desc = vars[i]["value"].asString();
		auto var = CreateVariable(type, name, value_desc);
		if (var.IsNone()) {
			return LoadSaveState::ERR_VAR;
		}
		g->AddVariable(name, var);
	}
	
	// 解析node并创建node
	auto nodes = root["nodes"];
	for (int i = 0; i < nodes.size(); ++i) {
		/*
		节点属性
			节点类型
			节点ID
			节点名称
			使用模块函数名
		UI属性
			节点位置
		*/
		int id = nodes[i]["id"].asInt();
		BpNodeType t = (BpNodeType)nodes[i]["type"].asInt();
		if (t == BpNodeType::BP_NODE_NORMAL) {
			auto node_name = nodes[i]["name"].asString();
			auto func_name = nodes[i]["func"].asString();
			auto node = SpawnNode(func_name);
			if (node == nullptr) {
				LOG(ERROR) << "load node " << func_name << " failed";
				continue;
			}
			g->AddNode(node);
		}
		if (t == BpNodeType::BP_NODE_EV) {
			// 从BpNodeLib中创建事件Node
			// TODO
		}
		if (t == BpNodeType::BP_NODE_VAR) {
			// 从BpNodeLib中创建变量Node
			// TODO
		}
		if (t == BpNodeType::BP_GRAPH || t == BpNodeType::BP_GRAPH_EXEC) {
			// TODO
			LOG(ERROR) << "Unimp";
		}
		// ???
		g->SetNextID(id);
	}
	
	// 解析link并创建link
	auto links = root["links"];
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
		g->AddLink(*sp, *ep);
		// link color
		std::vector<BpLink>& ls = g->GetLinks();
		auto colors = links[i]["color"];
		for (int j = 0; j < colors.size(); ++j) {
			ls.back().Color[j] = colors[j].asFloat();
		}
		// ???
		g->SetNextID(link_id);
	}

	g->SetNextID(root["max_id"].asInt());
	return LoadSaveState::OK;
}

LoadSaveState Bp::SaveGraph(const std::string& bp_json_path, const std::shared_ptr<BpGraph>& g) {
	std::ofstream ofs(bp_json_path);
	if (!ofs.is_open()) {
		LOG(ERROR) << "Open \"" << bp_json_path << "\" failed";
		return LoadSaveState::ERR_OPEN_FILE;
	}
	Json::Value root;
	auto res = SaveGraph(root, g);
	Json::StyledWriter sw;
	ofs << sw.write(root);
	ofs.close();
	return res;
}

LoadSaveState Bp::SaveGraph(Json::Value& root, const std::shared_ptr<BpGraph>& g) {
	root["version"] = Version();
	// ev_nodes
	auto& ev_nodes = g->GetEvNodes();
	for (auto ev : ev_nodes) {
		Json::Value json_evnode;
		json_evnode["name"] = ev.first;
		json_evnode["type"] = (int)ev.second->GetNodeType();
		json_evnode["style"] = (int)ev.second->GetNodeStyle();
		json_evnode["id"] = ev.second->GetID();
		root["ev_nodes"].append(json_evnode);
	}
	// var/base/nor nodes
	auto& nor_nodes = g->GetNodes();
	for (auto nor : nor_nodes) {
		Json::Value json_nornode;
		json_nornode["name"] = nor->GetName();
		json_nornode["type"] = (int)nor->GetNodeType();
		json_nornode["style"] = (int)nor->GetNodeStyle();
		json_nornode["id"] = nor->GetID();
		if (nor->GetNodeType() == BpNodeType::BP_NODE_VAR) {
			auto var_node = std::dynamic_pointer_cast<BpBaseNodeVar>(nor);
			json_nornode["get"] = var_node->IsGet();
		}
		root["nor_nodes"].append(json_nornode);
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
		root["links"].append(json_link);
	}
	// vars
	auto& vars = g->GetVariables();
	for (auto& x : vars) {
		Json::Value json_var;
		json_var["name"] = x.first;
		json_var["type"] = x.second.GetType();
		json_var["value"] = x.second.ToJson();
		root["vars"].append(json_var);
	}
	root["max_id"] = g->GetNextID();
	return LoadSaveState::OK;
}

std::shared_ptr<BpNode> Bp::SpawnNode(const std::string& node_name, const BpNodeType t) {
	// 从BpModLib中获得函数指针和描述
	// 从BpNodeLib中获得节点对象
	// 组合成Node添加到graph
	// _base_mods->GetMods()[0]->GetContents();
	if (t == BpNodeType::BP_NODE_NORMAL) {
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
				LOG(ERROR) << "create var " << func_info.type_args[i] << " failed";
				return nullptr;
			}
			res.emplace_back(var);
		}
		return _nodes_lib->CreateFuncNode(func_info, args, res);
	} else if (t == BpNodeType::BP_NODE_EV) {
		return _nodes_lib->CreateEvNode(node_name);
	} else if (t == BpNodeType::BP_NODE_BASE) {
		return _nodes_lib->CreateBaseNode(node_name);
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
