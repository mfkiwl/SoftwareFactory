#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

#include "bpcommon.hpp"
#include "SFEPanel.hpp"
#include "SFEPlugLib.hpp"
#include "SFERateCtrl.hpp"

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

    bool Sleep() { return _ratectrl.sleep(); }

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
    const std::shared_ptr<SFEPanel> GetPanel(const std::string& name);
    void ProcEditorMessage(const SFEMessage& msg);

    std::vector<SFEMessage> _send_que;
    bool _show_demo = false;
    std::unique_ptr<SFEPlugLib> _pluglib;
    sfe::RateCtl _ratectrl;
};

} // namespace sfe