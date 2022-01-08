#ifndef __BpVariable_hpp__
#define __BpVariable_hpp__
#include <memory>
#include <string>

#include <google/protobuf/message.h>

#include "BpModule.hpp"

namespace bp {

class BpVariable
{
public:
	BpVariable();
	// 引用传递
	BpVariable(const std::string& vt, const std::string& name, const pb_msg_ptr_t& v);
	BpVariable(const BpVariable& v){ operator=(v); }
	BpVariable& operator=(const BpVariable& o);
	BpVariable(BpVariable&& v);

	bool IsNone() { return _var == nullptr; }

	template<typename T>
	const std::shared_ptr<T> Get() { return std::dynamic_pointer_cast<T>(_var); }

	const std::string& GetName() const { return _var_name; }

	const std::string& GetType() const { return _var_type; }

	bool IsSameType(const BpVariable& v) { return _var_type == v._var_type; }

	// 值传递
	bool SetValue(const std::string& json_desc);
	bool SetValue(const pb_msg_ptr_t& v);

	// 引用传递
	void SetValueByRef(const pb_msg_ptr_t& v);

private:
	std::string _var_type;
	std::string _var_name;
	pb_msg_ptr_t _var;
};

} // namespace bp
#endif