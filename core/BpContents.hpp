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

    std::string PrintContents() {
        std::string content_str;
        TraverseContents(shared_from_this(), 0, content_str);
        return content_str;
    }

private:
    void TraverseContents(std::shared_ptr<BpContents> c, int depth, std::string& content_str) {
        if (c == nullptr) {
            return;
        }
        std::string pre = "";
        for (int i = 0; i < depth; ++i) {
            if (i == depth -1) {
                pre += "|--";
            } else {
                pre += "|  ";
            }
        }
        content_str += pre;
        content_str += c->_name;
        if (c->IsLeaf()) {
            content_str += c->IsFunc() ? "(func)" : "(var)";
        }
        content_str += "\n";
        for (int i = 0; i < c->_next_contents.size(); ++i) {
            TraverseContents(c->_next_contents[i], depth + 1, content_str);
        }
    }

    Type _type;
    std::string _name;
    std::weak_ptr<BpContents> _parent;
    std::vector<std::shared_ptr<BpContents>> _next_contents;
};

#endif