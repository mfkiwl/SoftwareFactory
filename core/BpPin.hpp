#ifndef __BpPin_hpp__
#define __BpPin_hpp__
#include <string>
#include <memory>

#include <glog/logging.h>
#include <google/protobuf/message.h>

#include "BpVariable.hpp"

namespace bp {

/// pin种类
enum class BpPinKind : int {
	BP_NONE,    ///< 未知
    BP_INPUT,   ///< 输入pin
    BP_OUTPUT,  ///< 输出pin
};

/// pin类型
enum class BpPinType : int {
	BP_NONE,    ///< 未知
	BP_FLOW,    ///< 执行流pin
	BP_VALUE,   ///< 变量pin
};

class BpNode;
class BpPin final
{
public:
	BpPin();
	BpPin(std::shared_ptr<BpNode> n, 
			BpPinKind k, BpPinType t, int id,
			const std::string& name, const BpVariable& v);
	BpPin(const BpPin& o) { operator=(o); }
	BpPin& operator=(const BpPin& o);
	bool operator==(const BpPin& o) {
		return ID == o.ID;
	}
	~BpPin();

	/**
	 * @brief 判断该Pin是否执行过
	 * 
	 * @return true 
	 * @return false 
	 */
	bool IsVaild() const { return _vaild; }
	void SetVaild(bool b) { _vaild = b; }

	/**
	 * @brief 判断pin是否有连线
	 * 
	 * @return true 
	 * @return false 
	 */
	bool IsLinked() const { return _linked; }
	void SetLinked(bool b = true) { _linked = b; }

	/**
	 * @brief 设置该Pin是否可执行
	 * 
	 * @param[in] b
	 * @note 仅针对BpPinType::BP_FLOW类型的pin
	 */
	void SetExecutable(bool b = true) { _executable = b; }
	bool IsExecutable() const { return _executable; }

	/**
	 * @brief 获得pin名称
	 */
	const std::string& GetName() { return _name; }

	/**
	 * @brief 设置pin名称
	 */
	void SetName(const std::string& name) { _name = name; }

	bool AssignByRef() { return _assign_by_ref; }
	void SetAssignByRef(bool b) { _assign_by_ref = b; }
	
	/**
	 * @brief 判断pin是否是值类型
	 * 
	 * @return true 
	 * @return false 
	 */
	bool IsValue() {
		return (_type == BpPinType::BP_VALUE);
	}

	/**
	 * @brief 获得proto变量对象
	 * 
	 * @tparam T proto变量类型
	 * @return std::shared_ptr<T> proto变量对象
	 */
	template<typename T>
	std::shared_ptr<T> Get() { return _v.Get<T>(); }
	
	/**
	 * @brief 获得BpVariable类型变量
	 * 
	 * @return BpVariable& BpVariable类型变量
	 */
	BpVariable& GetBpValue() { return _v; }
	
	/**
	 * @brief 设置变量
	 * 
	 * @param v 用json格式的字符串描述的变量
	 * @param b 见 IsVaild()
	 */
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
	
	/**
	 * @brief 获得pin种类
	 * 
	 * @return BpPinKind 输入/输出
	 */
	BpPinKind GetPinKind() const { return _kind; }

	/**
	 * @brief 判断pin类型相同
	 * 
	 * @param p pin指针
	 * @return true 
	 * @return false 
	 */
	bool IsSameType(BpPin* p);

	/**
	 * @brief 获得pin类型
	 * 
	 * @return BpPinType
	 */
	BpPinType GetPinType() const { return _type; }

	/**
	 * @brief 设置pin类型
	 * 
	 * @param t BpPinType
	 */
	void SetPinType(BpPinType t) { _type = t; }

	/**
	 * @brief 获得该pin的变量类型
	 * 
	 * @return const std::string& 
	 * @note 仅针对BpPinType::BP_VALUE类型的pin
	 */
	const std::string& GetVarType() { return _v.GetType(); }

	/**
	 * @brief 获得所属节点对象
	 * 
	 * @return const std::shared_ptr<BpNode> 节点对象
	 */
	const std::shared_ptr<BpNode> GetObj() { return _node.lock(); }

	/// pin ID
    int           ID;

	/**
	 * @brief 获得图对象的pin ID
	 * 
	 * @return int 
	 * @note 仅用于graph的输入输出节点
	 */
	int GetGraphPinID() { return _graph_pin_id; }

	/**
	 * @brief 设置图对象的pin ID
	 * 
	 * @param id 图对象id
	 * @note 仅用于graph的输入输出节点
	 */
	void SetGraphPinID(int id) { _graph_pin_id = id; }

private:
	bool          _executable;
	bool          _linked;
	bool          _vaild;
	bool          _assign_by_ref;

	std::string           _name;
    BpPinType             _type;
    BpPinKind             _kind;
	std::weak_ptr<BpNode> _node;
	int                   _graph_pin_id;
	BpVariable            _v;
};

} // namespace bp
#endif