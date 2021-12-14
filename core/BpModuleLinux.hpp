#ifndef __BP_MODULE_LINUX_HPP__
#define __BP_MODULE_LINUX_HPP__
#include <dlfcn.h>
#include <vector>
#include <string>
#include "BpModule.hpp"

namespace bp {

class BpModuleLinux : public BpModule
{
public:
    BpModuleLinux() = default;
    virtual ~BpModuleLinux();

protected:
    virtual bool Init(const char* dll_path) override;

    virtual void* GetFunc(const std::string& func_name) override;

private:
    int OpenDll(const std::string& dll_file);
    int GetDllSymbol(const char* cmd);
    std::string FilterSymbol(const std::string& line);

    void* _dl = nullptr;
    std::vector<std::string> _symbols;
};

} // namespace bp

#endif