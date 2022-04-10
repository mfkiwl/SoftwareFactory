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
    const std::shared_ptr<SFEPanel> GetPanel(const std::string& name);
    void ProcEditorMessage(const SFEMessage& msg);

    const std::string _name = "editor";
    bool _show_demo = false;
    std::unique_ptr<SFEPlugLib> _pluglib;
    std::vector<SFEMessage> _recv_que;
    sfe::RateCtl _ratectrl;

};

} // namespace sfe