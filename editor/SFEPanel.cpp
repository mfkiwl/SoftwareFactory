#include "SFEPanel.hpp"

namespace sfe {

void SFEPanel::ProcMessage() {
    for (auto it = _recv_que.begin(); it != _recv_que.end(); ++it) {
        OnMessage(*it);
    }
    _recv_que.clear();
}

} // namespace sfe