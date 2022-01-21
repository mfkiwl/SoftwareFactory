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

namespace sfe {

enum UILogLv : int {
    INFO,
    WARNING,
    ERROR,
};

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

/*
    初始化
    输入系统
    命令
        {
            command: "spawnnode",
            arg1: "bpmath.add_int",
            ...
        }
    更新
    退出

    面板公共设置
*/
class SFEPanel : public std::enable_shared_from_this<SFEPanel> 
{
public:
    const std::string& PanelName() { return _name; }
    void SetPanelName(const std::string& name) { _name = name; }

    virtual bool Init() = 0;

    virtual void ProcEvent(const SDL_Event event) {}

    virtual void Update() = 0;

    virtual void Exit() = 0;

    void UILog(const std::string& msg, UILogLv lv = UILogLv::INFO);

    /* Call by SFEditor */
    void ProcMessage();
    void RecvMessage(const SFEMessage& msg) {
        _recv_que.emplace_back(msg);
    }
    const std::vector<SFEMessage>& GetDispatchMessage() { return _send_que; }
    void ClearDispathMessage() { _send_que.clear(); }
    /* Call by child */
    void SendMessage(const SFEMessage& msg) {
        if (msg.src == "uinodes") {
            LOG_EVERY_N(INFO, 30) << "per 30 msg: " << msg.Print();
        } else {
            LOG(INFO) << msg.Print();
        }
        _send_que.emplace_back(msg);
    }
protected:
    /* Call by this */
    virtual void OnMessage(const SFEMessage& msg) {}

private:
    std::string _name;
    std::vector<SFEMessage> _send_que;
    std::vector<SFEMessage> _recv_que;
};

} // namespace sfe