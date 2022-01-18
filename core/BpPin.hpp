#ifndef __BpPin_hpp__
#define __BpPin_hpp__
#include <string>
#include <memory>

#include <glog/logging.h>
#include <google/protobuf/message.h>

#include "BpVariable.hpp"

namespace bp {

enum class BpPinKind : int {
	BP_NONE,
    BP_INPUT,
    BP_OUTPUT,
};

enum class BpPinType : int {
	BP_NONE,
	BP_FLOW,
	BP_VALUE,
};

class BpObj;
class BpPin final
{
public:
	BpPin();
	BpPin(std::shared_ptr<BpObj> n, 
			BpPinKind k, BpPinType t, int id,
			const std::string& name, const BpVariable& v);

	BpPin(const BpPin& o) { operator=(o); }

	BpPin& operator=(const BpPin& o);

	~BpPin();

	bool IsValue() {
		return (_type == BpPinType::BP_VALUE);
	}

	bool IsSameType(BpPin* p);

	bool operator==(const BpPin& o) {
		return ID == o.ID;
	}

	const pb_msg_ptr_t GetValue() { return _v.Get<pb_msg_t>(); }
	void SetValue(const std::string& v, bool b = false/* 不要设置该值 */) {
		_vaild = b;
		_v.SetValue(v);
	}
	void SetValue(const Json::Value& v, bool b = false/* 不要设置该值 */) {
		_vaild = b;
		_v.SetValue(v);
	}
	void SetValue(const pb_msg_ptr_t& v, bool b = false/* 不要设置该值 */) {
		_vaild = b;
		_v.SetValue(v);
	}
	void SetValueByRef(const pb_msg_ptr_t& v, bool b = false/* 不要设置该值 */) {
		_vaild = b;
		_v.SetValueByRef(v);
	}
	/* 该Pin是否已经执行过 */
	bool IsVaild() { return _vaild; }
	void SetVaild(bool b) { _vaild = b; }

	bool IsLinked() const { return _linked; }
	void SetLinked(bool b = true) { _linked = b; }

	/* 设置该Pin是否可执行 */
	void SetExecutable(bool b = true) { _executable = b; }
	bool IsExecutable() { return _executable; }

	const std::string& GetName() { return _name; }
	void SetName(const std::string& name) { _name = name; }

	bool AssignByRef() { return _assign_by_ref; }
	void SetAssignByRef(bool b) { _assign_by_ref = b; }

	BpPinKind GetPinKind() const { return _kind; }
	BpPinType GetPinType() const { return _type; }
	const std::string& GetVarType() { return _v.GetType(); }
	void SetPinType(BpPinType t) { _type = t; }
	const std::shared_ptr<BpObj> GetObj() { return _node.lock(); }

    int           ID;

private:
	bool          _executable;
	bool          _linked;
	bool          _vaild;
	bool          _assign_by_ref;

	std::string           _name;
    BpPinType             _type;
    BpPinKind             _kind;
	std::weak_ptr<BpObj>  _node;

	BpVariable            _v;
};

} // namespace bp
#endif