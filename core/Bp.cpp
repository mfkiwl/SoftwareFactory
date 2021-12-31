#include <fstream>

#include "Bp.hpp"
#include "BpModLib.hpp"
#include "BpNodeLib.hpp"
#include "BpModLibLinux.hpp"
#include "BpGraph.hpp"

namespace bp {
	
Bp& Bp::Instance() {
	static Bp instance;
	return instance;
}

Bp::Bp() {
	_base_mods = std::make_shared<BpModLibLinux>();
	_base_mods->Init("../conf/");
}

Bp::~Bp() {}

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

LoadState Bp::LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g) {
	std::ifstream ifs(bp_json_path);
	if (!ifs.is_open()) {
		LOG(ERROR) << "Open \"" << bp_json_path << "\" failed";
		return LoadState::ERR_OPEN_FILE;
	}
	Json::Value value;
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(ifs, value)) {
		LOG(ERROR) << "Parse \"" << bp_json_path << "\" failed";
        return LoadState::ERR_PARSE;
    }
	LOG(INFO) << "Load \"" << bp_json_path << "\"...";
	return LoadGraph(value, g);
}

LoadState Bp::LoadGraph(const Json::Value& root, std::shared_ptr<BpGraph>& g) {
	// 检查配置文件版本是否小于当前程序版本
	auto bin_ver = bp::Bp::Instance().Version();
	if (bin_ver != root["version"].asString()) {
		LOG(ERROR) << "Version error: bin " << bin_ver << ", conf \""
			<< root["version"].asString();
		return LoadState::ERR_VERSION;
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
			return LoadState::ERR_VAR;
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
		BpObjType t = (BpObjType)nodes[i]["type"].asInt();
		if (t == BpObjType::BP_NODE_NORMAL) {
			auto node_name = nodes[i]["name"].asString();
			auto func_name = nodes[i]["func"].asString();
			// 从BpModLib中获得函数指针
			// 从BpNodeLib中获得节点对象
			// 组合成Node添加到graph
			// TODO
		}
		if (t == BpObjType::BP_NODE_EV) {
			// 从BpNodeLib中创建事件Node
			// TODO
		}
		if (t == BpObjType::BP_NODE_VAR) {
			// 从BpNodeLib中创建变量Node
			// TODO
		}
		if (t == BpObjType::BP_GRAPH || t == BpObjType::BP_GRAPH_EXEC) {
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
			return LoadState::ERR_SEARCH_PIN;
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
	return LoadState::OK;
}

void Bp::SaveGraph(const std::shared_ptr<BpGraph>& g, const std::string& file) {
	// TODO
	// pt::ptree tree;
	// tree.add("version", Version());
	// /*
	// links
	// nodes:
	// 	ev_nodes
	// 	nor_nodes
	// variables
	// next_id
	// */
	// BpGraph* g = m_graph;

	// pt::ptree nodes;
	// auto& ev_nodes = m_graph->GetEvNodes();
	// for (auto p : ev_nodes) {
	// 	pt::ptree node;
	// 	node.add("node_kind", (int)p.second->GetComponentType());
	// 	node.add("id", p.second->GetID());
	// 	node.add("name", p.first);
	// 	node.add("class_name", p.second->GetComeFrom().first);
	// 	node.add("model_name", p.second->GetComeFrom().second);
	// 	// pin
	// 	// 目前不用保存pin的属性,node中都已经知道如何创建pin了。
	// 	nodes.push_back(std::make_pair("", node));
	// }
	// auto& nor_nodes = m_graph->GetNodes();
	// for (auto n : nor_nodes) {
	// 	pt::ptree node;
	// 	node.add("node_kind", (int)n->GetComponentType());
	// 	node.add("id", n->GetID());
	// 	node.add("name", n->GetName());
		
	// 	if (n->GetComponentType() == BpComponentType::BP_NODE_VAR || n->GetComponentType() == BpComponentType::BP_NODE_REF_VAR) {
	// 		BpVariableNode* var_node = static_cast<BpVariableNode*>(n);
	// 		node.add("getset", (int)var_node->m_getset_type);
	// 	}
	// 	else {
	// 		node.add("class_name", n->GetComeFrom().first);
	// 		node.add("model_name", n->GetComeFrom().second);
	// 	}
	// 	nodes.push_back(std::make_pair("", node));
	// }
	// tree.add_child("nodes", nodes);

	// pt::ptree links;
	// for (auto& l : m_graph->GetLinks()) {
	// 	pt::ptree link;
	// 	link.add("id", l.ID);
	// 	link.add("start_id", l.StartPinID);
	// 	link.add("end_id", l.EndPinID);
	// 	pt::ptree color;
	// 	color.push_back(std::make_pair("", pt::ptree().put("", l.Color[0])));
	// 	color.push_back(std::make_pair("", pt::ptree().put("", l.Color[1])));
	// 	color.push_back(std::make_pair("", pt::ptree().put("", l.Color[2])));
	// 	color.push_back(std::make_pair("", pt::ptree().put("", l.Color[3])));
	// 	link.push_back(std::make_pair("color", color));
	// 	links.push_back(std::make_pair("", link));
	// }
	// tree.add_child("links", links);

	// pt::ptree variables;
	// for (auto& x : m_graph->GetValues()) {
	// 	if(x.second.IsRef()) continue;
	// 	pt::ptree var;
	// 	var.add("name", x.first);
	// 	var.add("value", x.second.ToStr());
	// 	variables.push_back(std::make_pair("", var));
	// }
	// tree.add_child("variables", variables);
	// tree.add("max_id", g->GetNextID());
	// return tree;
}

} // nemespace bp
