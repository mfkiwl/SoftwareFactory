#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>

#include "imgui.h"

#define SFE_PANEL_TYPE \
public: \
virtual std::string TypeName() { return typeid(*this).name(); }

namespace sfe {

struct SFEMessage {
    std::string src;
    std::string dst;
    std::string msg;
    Json::Value json_msg;

    std::string Print() const {
        std::stringstream ss;
        ss << "{src: \"" << src << "\", dst: \"" << dst << "\", msg: \"" 
            << msg << ", json msg: "
            << json_msg.toStyledString() << "}";
        return ss.str();
    }
};

class SFEPanel : public std::enable_shared_from_this<SFEPanel> 
{
    friend class SFEditor;
public:
    const std::string& PanelName() { return _name; }
    void SetPanelName(const std::string& name) { _name = name; }

    virtual bool Init() = 0;

    virtual void ProcEvent(const SDL_Event event) {}

    virtual void Update() = 0;

    virtual void Exit() = 0;

    void SetShow(bool b) { _show = b; }
    bool IsShow() { return _show; }
    
    void SendMessage(const SFEMessage& msg);

    static bool RegPanel(const std::string& panel_name, std::shared_ptr<SFEPanel> p);

    static std::shared_ptr<SFEPanel> GetPanel(const std::string& panel_name);

    static const std::unordered_map<std::string, std::shared_ptr<SFEPanel>>& GetPanels();

    static void ClearPanels() { _s_panels.clear(); }

protected:
    virtual void OnMessage(const SFEMessage& msg) {}
    virtual void OnPostSendMessage(const SFEMessage& msg) {}

private:
    /* Call by SFEditor */
    void ProcMessage();
    void RecvMessage(const SFEMessage& msg) {
        _recv_que.emplace_back(msg);
    }
    const std::vector<SFEMessage>& GetDispatchMessage() { return _send_que; }
    void ClearDispathMessage() { _send_que.clear(); }

    bool _show = true;
    std::string _name;
    std::vector<SFEMessage> _send_que;
    std::vector<SFEMessage> _recv_que;

    static std::unordered_map<std::string, std::shared_ptr<SFEPanel>> _s_panels;
};

} // namespace sfe