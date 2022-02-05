#include <sstream>
#include "SFEPanel.hpp"
#include "bpcommon.hpp"

namespace sfe {

std::unordered_map<std::string, std::shared_ptr<SFEPanel>> SFEPanel::_s_panels;

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