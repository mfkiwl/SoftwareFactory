#pragma once
#include "SFEPanel.hpp"

typedef std::shared_ptr<sfe::SFEPanel> (*create_panel_func_t)(const std::string&);
namespace sfe {

class SFEPlug {
public:
    SFEPlug() = default;
    virtual ~SFEPlug();

    bool Init(const char* dll_path);

    create_panel_func_t GetCreateFunc() { return _create_func; }
    
private:
    int OpenDll(const std::string& dll_file);
    void* GetFunc(const std::string& func_name);

    create_panel_func_t _create_func = nullptr;
    void* _dl = nullptr;
};

} // namespace sfe