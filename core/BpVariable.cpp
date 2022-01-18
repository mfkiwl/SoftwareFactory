#include <glog/logging.h>

#include "bpcommon.hpp"
#include "BpVariable.hpp"

namespace bp {

BpVariable::BpVariable()
	: _var_type("")
	, _var_name("")
	, _var(nullptr)
{}

BpVariable::BpVariable(const std::string& vt, const std::string& name, const pb_msg_ptr_t& v) 
	: _var_type(vt)
	, _var_name(name)
	, _var(v)
{}

BpVariable::BpVariable(BpVariable&& v) {
	_var_name = std::move(v._var_name);
	_var_type = std::move(v._var_type);
	_var = v._var;
	v._var = nullptr;
}

BpVariable& BpVariable::operator=(const BpVariable& o) 
{
	_var_name = o._var_name;
	_var_type = o._var_type;
	_var = o._var;
	return *this;
}

bool BpVariable::SetValue(const pb_msg_ptr_t& v) {
	if (v == nullptr) {
		LOG(ERROR) << "pb value is nullptr";
		return false;
	}
	Json::Value json_v;
	// FIXME: 值转换会降低效率, 直接赋值可能会导致未知的问题
	JsonPbConvert::PbMsg2Json(*v, json_v);
	return SetValue(json_v);
}

bool BpVariable::SetValue(const std::string& v) {
	Json::Value value;
	Json::Reader reader(Json::Features::strictMode());
	if (!reader.parse(v, value)) {    
		LOG(ERROR) << "parse json failed";
		return false;
	}
	return SetValue(value);
}

bool BpVariable::SetValue(const Json::Value& v) {
	if (v.isNull() || _var == nullptr) {
		LOG(ERROR) << "json_value is empty";
		return false;
	}
	JsonPbConvert::Json2PbMsg(v, *_var);
	return true;
}

void BpVariable::SetValueByRef(const pb_msg_ptr_t& v) {
	_var = v;
}

} // namespace bp