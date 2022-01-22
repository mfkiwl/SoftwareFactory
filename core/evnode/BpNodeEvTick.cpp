#include "BpNodeEvTick.hpp"

namespace bp {

BpNodeEvTick::BpNodeEvTick(std::shared_ptr<BpGraph> parent)
	: BpNodeEv("Tick", parent, 1, -1)
{}

void BpNodeEvTick::Logic() {
}

} // namespace bp