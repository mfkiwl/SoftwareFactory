#include <sstream>
#include "SFEPanel.hpp"
#include "bpcommon.hpp"

namespace sfe {

//// static member
std::unordered_map<std::string, std::shared_ptr<SFEPanel>> SFEPanel::_s_panels;

bool SFEPanel::RegPanel(const std::string& panel_name, std::shared_ptr<SFEPanel> p) {
    p->SetPanelName(panel_name);
    if (_s_panels.find(p->PanelName()) == _s_panels.end()) {
        if (!p->Init()) {
            LOG(ERROR) << "Panel " << panel_name << " failed";
            return false;
        }
        _s_panels[p->PanelName()] = p;
        return true;
    }
    return false;
}

std::shared_ptr<SFEPanel> SFEPanel::GetPanel(const std::string& panel_name) {
    if (_s_panels.find(panel_name) == _s_panels.end()) {
        return nullptr;
    }
    return _s_panels[panel_name];
}

const std::unordered_map<std::string, std::shared_ptr<SFEPanel>>& SFEPanel::GetPanels() {
    return _s_panels;
}
//// end static member

void SFEPanel::ProcMessage() {
    for (auto it = _recv_que.begin(); it != _recv_que.end(); ++it) {
        OnMessage(*it);
    }
    _recv_que.clear();
}

/* Call by child */
void SFEPanel::SendMessage(const SFEMessage& msg) {
    if (msg.src == "uinodes") {
        LOG_EVERY_N(INFO, 30) << "per 30 msg: " << msg.Print();
    } else {
        LOG(INFO) << msg.Print();
    }
    _send_que.emplace_back(msg);
    OnPostSendMessage(msg);
}

} // namespace sfe