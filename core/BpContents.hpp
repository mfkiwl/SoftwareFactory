#ifndef __BPCONTENTS_HPP__
#define __BPCONTENTS_HPP__
#include <string>
#include <vector>

class BpContents {
public:
    enum class Type {
        CONTENTS,
        FUNC,
        VAL,
    };

    BpContents(BpContents* parent, Type t, std::string name)
        : _parent(parent)
        , _type(t)
        , _name(name)
    {}
    
    bool AddChild(BpContents& contents) {
        _next_contents.emplace_back(contents);
        return true;
    }

    void SetParent(BpContents* parent) {
        _parent = parent;
    }

    std::string GetFullPath() {
        if (_parent == nullptr) {
            return _name;
        }
        return _parent->GetFullPath() + "." + _name;
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
    BpContents* _parent;
    std::vector<BpContents> _next_contents;
};

#endif