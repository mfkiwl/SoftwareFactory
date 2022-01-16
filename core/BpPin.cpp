#include <glog/logging.h>
#include "BpPin.hpp"

namespace bp {

BpPin::BpPin() 
	: _kind(BpPinKind::BP_NONE)
	, _type(BpPinType::BP_NONE)
	, ID(-1)
{}

BpPin::BpPin(std::shared_ptr<BpObj> n, 
		BpPinKind k, BpPinType t, int id, 
		const std::string& name, const BpVariable& v)
	: _v(v)
	, ID(id)
	, _linked(false)
	, _name(name)
	, _node(n)
	, _type(t)
	, _kind(k)
	, _vaild(false)
	, _executable(true)
{
	LOG(INFO) << "Create pin " << id;
}

BpPin& BpPin::operator=(const BpPin& o) {
	LOG(INFO) << "Copy/= pin from " << o.ID << " to " << ID << std::endl;
	ID = o.ID;
	_executable = o._executable;
	_linked = o._linked;
	_name = o._name;
	_type = o._type;
	_kind = o._kind;
	_node = o._node.lock();
	_v = o._v;
	return *this;
}

BpPin::~BpPin() {
	LOG(INFO) << "Delete pin " << ID << std::endl;
}

bool BpPin::IsSameType(BpPin* p)
{
	if (_type == BpPinType::BP_FLOW && p->_type == BpPinType::BP_FLOW) return true;
	return p->GetVarType() == GetVarType();
}

} // namespace bp