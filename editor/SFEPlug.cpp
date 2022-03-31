#include "SFEPlug.hpp"

#include <dlfcn.h>

namespace sfe {

SFEPlug::~SFEPlug() {
    if (_dl != nullptr) {
        dlclose(_dl);
        _dl = nullptr;
    }
}

bool SFEPlug::Init(const char* dll_path) {
    if (OpenDll(dll_path)) {
        return false;
    }
    _create_func = GetFunc("create_panel");
    if (_create_func == nullptr) {
        return false;
    }
    return true;
}

void* SFEPlug::GetFunc(const std::string& func_name) {
    LOG(INFO) << "load func \"" << func_name << "\" : " << func_name;
    return dlsym(_dl, func_name.c_str());
}

int SFEPlug::OpenDll(const std::string& dll_file) {
    const char* dll_path = dll_file.c_str();
    _dl = dlopen(dll_path, RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);
    if(nullptr == _dl) {
        return -1;
    }
    return 0;
}

} // namespace sfe