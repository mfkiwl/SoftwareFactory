#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

#include "bpcommon.hpp"
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanel;
class SFEditor {
public:
    bool Init();

    void ProcEvent(const SDL_Event event);

    void Update();

    void DispatchMessage();

    void ProcMessage();

    void Exit();

private:
    const std::string _name = "editor";
    void SendMessage(const SFEMessage& msg) {
        if (msg.src == "uinodes") {
            LOG_EVERY_N(INFO, 30) << "per 30 msg: " << msg.Print();
        } else {
            LOG(INFO) << msg.Print();
        }
        _send_que.emplace_back(msg);
    }
    void UILog(const std::string& msg, UILogLv lv) {
        std::stringstream ss;
        static std::string str_lv = "INFO";
        if (lv == UILogLv::ERROR) {
            str_lv = "ERROR";
        } else if (lv == UILogLv::WARNING) {
            str_lv = "WARNING";
        }
        ss << str_lv << " ";
        ss << bp::BpCommon::GetCurTime() << "] ";
        ss << msg << "\n";
        SendMessage({_name, "log", ss.str(), Json::Value::null});
    }
    const std::shared_ptr<SFEPanel> GetPanel(const std::string& name);
    void ProcEditorMessage(const SFEMessage& msg);

    std::vector<std::shared_ptr<SFEPanel>> _panels;
    std::vector<SFEMessage> _send_que;
    bool _show_demo = false;
};

} // namespace sfe