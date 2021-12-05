#ifndef __BP_MODULE_LINUX_HPP__
#define __BP_MODULE_LINUX_HPP__
#include "BpModule.hpp"

namespace bp {

class BpModuleLinux : public BpModule
{
public:
    BpModuleLinux();

protected:
    virtual bool Init(const std::string& json_path) override;

    virtual void* GetFunc(const std::string& func_name) override;
};

} // namespace bp

#endif