#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

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
    const std::shared_ptr<SFEPanel> GetPanel(const std::string& name);
    void ProcEditorMessage(const SFEMessage& msg);

    std::vector<std::shared_ptr<SFEPanel>> _panels;
    bool _show_demo = true;
};

} // namespace sfe