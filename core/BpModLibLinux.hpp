#pragma once

#include "BpModLib.hpp"

namespace bp {

class BpModLibLinux : public BpModLib
{
public:
    BpModLibLinux() = default;

protected:
    std::shared_ptr<BpModule> CreateModule(const std::string& mod_name) override;
};

} // namespace bp