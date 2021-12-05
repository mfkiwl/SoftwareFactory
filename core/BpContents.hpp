#ifndef __BPCONTENTS_HPP__
#define __BPCONTENTS_HPP__
#include <memory>
#include <string>
#include <vector>

class BpContents : public std::enable_shared_from_this<BpContents> {
public:
    enum class Type {
        CONTENTS,
        FUNC,
        VAL,
    };

    BpContents(std::shared_ptr<BpContents> parent, Type t, std::string name)
        : std::enable_shared_from_this<BpContents>()
        , _parent(parent)
        , _type(t)
        , _name(name)
    {}
    
    bool AddChild(std::shared_ptr<BpContents> contents) {
        if (contents == nullptr) {
            return false;
        }
        contents->_parent.reset();
        contents->_parent = shared_from_this();
        _next_contents.emplace_back(contents);
        return true;
    }

    bool SetParent(std::shared_ptr<BpContents> parent) {
        if (parent == nullptr) {
            return false;
        }
        parent->AddChild(shared_from_this());
        return true;
    }

    std::string GetName() { 
        return _name;
    }

    std::string GetFullPath() {
        if (_parent.expired()) {
            return _name;
        }
        return _parent.lock()->GetFullPath() + "." + _name;
    }

    bool HasChild() {
        return !_next_contents.empty();
    }

    bool IsLeaf() {
        return _type != Type::CONTENTS;
    }

    bool IsFunc() {
        return _type == Type::FUNC;
    }

    bool IsVal() {
        return _type == Type::VAL;
    }

private:
    Type _type;
    std::string _name;
    std::weak_ptr<BpContents> _parent;
    std::vector<std::shared_ptr<BpContents>> _next_contents;
};

#endif