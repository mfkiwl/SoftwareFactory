#include <sstream>
#include "SFEPanel.hpp"
#include "bpcommon.hpp"

namespace sfe {

void SFEPanel::ProcMessage() {
    for (auto it = _recv_que.begin(); it != _recv_que.end(); ++it) {
        OnMessage(*it);
    }
    _recv_que.clear();
}

void SFEPanel::UILog(const std::string& msg, UILogLv lv) {
    std::stringstream ss;
    static std::string str_lv = "INFO";
    if (lv == UILogLv::ERROR) {
        str_lv = "ERROR";
    } else if (lv == UILogLv::WARNING) {
        str_lv = "WARNING";
    }
    ss << str_lv << " ";
    ss << bp::BpCommon::GetCurTime() << "] ";
    ss << msg;
    SendMessage({PanelName(), "log", ss.str(), Json::Value()});
}

} // namespace sfe