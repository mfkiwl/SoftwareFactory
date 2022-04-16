#include "SFEParams.hpp"
#include <vector>
#include <iostream>
#include "bpcommon.hpp"
#include "bpflags.hpp"

namespace sfe {

SFEParams& SFEParams::Instance() {
	static SFEParams instance;
	return instance;
}

void SFEParams::LoadParams() {
    _params = bp::BpCommon::LoadJsonFromFile(bp::FLAGS_conf_dir + bp::FLAGS_sfe_params_file);
}

void SFEParams::LoadParams(const std::string& json_str) {
    _params = bp::BpCommon::Str2Json(json_str);
}

bool SFEParams::SaveParams() {
    return bp::BpCommon::SaveJsonToFile(_params, bp::FLAGS_conf_dir + bp::FLAGS_sfe_params_file);
}

std::vector<std::string> SFEParams::SplitKeys(const std::string& keys) {
    std::vector<std::string> res;
    std::string tmp = "";
    for (int i = 0; i < keys.size(); ++i) {
        if (keys[i] == '.') {
            res.push_back(tmp);
            tmp.clear();
        } else {
            tmp += keys[i];
        }
    }
    if (!tmp.empty()) {
        res.push_back(tmp);
    }
    return res;
}

} // namespace sfe 