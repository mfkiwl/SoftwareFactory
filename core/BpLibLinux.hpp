#ifndef __BP_LIB_LINUX_HPP__
#define __BP_LIB_LINUX_HPP__
#include "BpLib.hpp"

namespace bp {

class BpLibLinux : public BpLib
{
public:
    BpLibLinux() = default;

protected:
    std::vector<std::string> GetDirFiles(const std::string& conf_path) override;
    std::shared_ptr<BpModule> CreateModule(const std::string& mod_name) override;
};

} // namespace bp

#endif