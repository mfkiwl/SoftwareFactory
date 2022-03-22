#ifndef __BPCONTENTS_HPP__
#define __BPCONTENTS_HPP__
#include <memory>
#include <string>
#include <vector>

class BpContents : public std::enable_shared_from_this<BpContents> {
public:
    enum class Type : int {
        CONTENTS,
        LEAF,
        
    };
    enum class LeafType : int {
        NONE,
        FUNC,
        VAL,
        EV,
        BASE,
        USER,
        GRAPH,
    };

    BpContents(std::shared_ptr<BpContents> parent, std::string name, Type t, LeafType lt = LeafType::NONE)
        : std::enable_shared_from_this<BpContents>()
        , _parent(parent)
        , _type(t)
        , _leaf_type(lt)
        , _name(name)
    {}
    
    const std::vector<std::shared_ptr<BpContents>>&
    GetChildren() { return _next_contents; }

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

    const std::string& GetName() { 
        return _name;
    }

    std::string GetFullPath() {
        if (_parent.expired() || _parent.lock()->GetName().empty()) {
            return _name;
        }
        return (_parent.lock()->GetFullPath() + "." + _name);
    }

    bool HasChild() {
        return !_next_contents.empty();
    }

    bool IsLeaf() {
        return _type != Type::CONTENTS;
    }

    Type GetType() {
        return _type;
    }
    LeafType GetLeafType() {
        return _leaf_type;
    }

    std::string PrintContents() {
        std::string content_str;
        TraverseContents(shared_from_this(), 0, content_str);
        return content_str;
    }

    void SetData(const std::string& data) { _data = data; }
    const std::string& GetData() { return _data; }

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
        content_str += "\n";
        for (int i = 0; i < c->_next_contents.size(); ++i) {
            TraverseContents(c->_next_contents[i], depth + 1, content_str);
        }
    }

    Type _type;
    LeafType _leaf_type;
    std::string _name;
    std::string _data;
    std::weak_ptr<BpContents> _parent;
    std::vector<std::shared_ptr<BpContents>> _next_contents;
};

#endif