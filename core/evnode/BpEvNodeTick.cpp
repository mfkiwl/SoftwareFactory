#include "BpEvNodeTick.hpp"

namespace bp {

BpEvNodeTick::BpEvNodeTick(std::shared_ptr<BpGraph> parent)
	: BpEvNode("Tick", parent, 1, -1)
{}

void BpEvNodeTick::Logic() {
}

} // namespace bp