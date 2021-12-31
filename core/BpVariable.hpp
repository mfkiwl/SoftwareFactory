#ifndef __BpVariable_hpp__
#define __BpVariable_hpp__
#include <memory>
#include <string>

#include <google/protobuf/message.h>

namespace bp {

class BpVariable
{
	typedef std::shared_ptr<google::protobuf::Message> var_t;
public:
	BpVariable();
	// 引用传递
	BpVariable(const std::string& vt, const std::string& name, var_t& v);
	BpVariable(const BpVariable& v){ operator=(v); }
	BpVariable& operator=(const BpVariable& o);
	BpVariable(BpVariable&& v);

	bool IsNone() { return _var == nullptr; }

	const var_t& Get() { return _var; }

	std::string& GetName() { return _var_name; }

	std::string& GetType() { return _var_type; }

	bool IsSameType(const BpVariable& v) { return _var_type == v._var_type; }

	// 值传递
	bool SetValue(const std::string& json_desc);
	bool SetValue(const var_t& v);

	// 引用传递
	void SetValueByRef(const var_t& v);

private:
	std::string _var_type;
	std::string _var_name;
	var_t _var;
};

} // namespace bp
#endif