#include "SFEPanel.hpp"

namespace sfe {

class @template_name@ : public SFEPanel {
public:
    bool Init() override {
        return true;
    }

    void Update() override {
        ImGui::Begin(PanelName().c_str());

        ImGui::Button("Hello");
        
        ImGui::End();
    }

    void Exit() override {

    }

    void OnMessage(const SFEMessage& msg) override {

    }
};

} // namespace sfe

extern "C" std::shared_ptr<sfe::SFEPanel> create_panel(const std::string& panel_type) {
    // TODO here
    return std::make_shared<sfe::@template_name@>();
}
