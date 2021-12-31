#pragma once

#include "BpModLib.hpp"

namespace bp {

class BpModLibLinux : public BpModLib
{
public:
    BpModLibLinux() = default;

protected:
    std::vector<std::string> GetDirFiles(const std::string& conf_path) override;
    std::shared_ptr<BpModule> CreateModule(const std::string& mod_name) override;
};

} // namespace bp