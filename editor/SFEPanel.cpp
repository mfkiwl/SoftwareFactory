#include <sstream>
#include "SFEPanel.hpp"
#include "bpcommon.hpp"

namespace sfe {

//// static member
std::unordered_map<std::string, std::shared_ptr<SFEPanel>> SFEPanel::_s_panels;
std::vector<SFEMessage> SFEPanel::_send_que;

void SFEPanel::SendMessage(const std::string& src, const std::string& dst, const std::string& msg1, const Json::Value& msg2) {
    auto p = GetPanel(dst);
    if (p == nullptr) {
        _send_que.push_back({src, dst, msg1, msg2});
        return;
    }
    p->RecvMessage({src, dst, msg1, msg2});
}

void SFEPanel::SendMessage(const std::string& src, const std::string& dst, const std::string& msg) {
    SendMessage(src, dst, msg, Json::Value::null);
}

void SFEPanel::SendMessage(const std::string& src, const std::string& dst, const Json::Value& msg) {
    SendMessage(src, dst, "", msg);
}

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

void SFEPanel::SendMessage(const std::string& dst, const std::string& msg) {
    SendMessage(_name, dst, msg, Json::Value::null);
}

void SFEPanel::SendMessage(const std::string& dst, const Json::Value& msg) {
    SendMessage(_name, dst, "", msg);
}

} // namespace sfe