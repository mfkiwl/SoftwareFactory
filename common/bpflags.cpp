#include "bpflags.hpp"

namespace bp {

DEFINE_string(base_mod_lib_dir, "/opt/SoftwareFactory/lib/", "基础模块配置文件目录");
DEFINE_string(base_mod_cfg_dir, "/opt/SoftwareFactory/conf/", "基础模块配置文件目录");
DEFINE_string(mod_graph_dir, "/opt/SoftwareFactory/mods/", "组织模块目录");
DEFINE_string(assets_dir, "/opt/SoftwareFactory/assets/", "资源目录");
DEFINE_string(conf_dir, "/opt/SoftwareFactory/params/", "配置文件目录");
DEFINE_string(plug_conf_dir, "/opt/SoftwareFactory/plug/", "配置文件目录");
DEFINE_string(sfe_params_file, "sfe.json", "编辑器配置文件");

} // namespace bp