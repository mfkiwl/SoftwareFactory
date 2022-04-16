#pragma once
#include <string>
#include <jsoncpp/json/json.h>
#include <glog/logging.h>

namespace sfe {

class SFEParams {
public:
    static SFEParams& Instance();

    const Json::Value& GetParam(const std::string& param_key) {
        auto keys = SplitKeys(param_key);
        return GetParamHelper(keys, 0, _params);
    }

    template<typename T>
    bool SetParam(const std::string& param_key, T value) {
        auto keys = SplitKeys(param_key);
        return SetParamHelper(keys, 0, _params, value);
    }

    bool SaveParams();

    std::string ToStr() { return _params.toStyledString(); }
    
    //// internal (just for unit test)
    void LoadParams();
    void LoadParams(const std::string& json_str);
    std::vector<std::string> SplitKeys(const std::string&);

private:
	SFEParams() {
        LoadParams();
    }
	~SFEParams() {}
    SFEParams(const SFEParams &o) {
        operator=(o);
    }
    const SFEParams &operator=(const SFEParams &o) {
        _params = o._params;
        return *this;
    }

    template<typename T>
    bool SetParamHelper(const std::vector<std::string> keys, int idx, Json::Value& src_v, T dst_v) {
        if (idx == keys.size()) {
            src_v = dst_v;
            return true;
        }
        if (src_v[keys[idx]].isNull()) {
            src_v[keys[idx]] = Json::Value();
        }
        return SetParamHelper(keys, idx + 1, src_v[keys[idx]], dst_v);
    }

    const Json::Value& GetParamHelper(const std::vector<std::string> keys, int idx, const Json::Value& src_v) {
        if (idx == keys.size()) {
            return src_v;
        }
        if (src_v.isNull()) {
            return Json::Value::null;
        }
        if (src_v[keys[idx]].isNull()) {
            return Json::Value::null;
        }
        return GetParamHelper(keys, idx + 1, src_v[keys[idx]]);
    }

    Json::Value _params;
};

} // namespace sfe 