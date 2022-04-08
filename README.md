# 软件工厂

# 功能概述
该工具库只是充当胶水的作用将各个模块组织起来生成一个更大的工具程序.  
工具库一共分为两部分:  
1. 运行软件的核心库以及多个基础模块; 
2. 软件工厂编辑器,主要用于编辑更高级的模块组件(或者可执行程序).

# 开发/运行环境
操作系统: Ubuntu 18.04+
开发语言：C++14

# 安装依赖
```sh
sudo apt-get install build-essential cmake
sudo apt-get install libjsoncpp-dev libprotobuf-dev protobuf-compiler libgflags-dev libgtest-dev libgoogle-glog-dev 
sudo apt-get install libsdl2-dev libglew-dev libgtk-3-dev
# ubuntu18.04需要编译安装gtest
cd /usr/src/gtest
sudo cmake CMakeLists.txt 
sudo make 
sudo cp *.a /usr/lib
```

# 构建
![build install gif](doc/pics/build_install_sf.gif)

```sh
mkdir build
cd build
cmake ../
make
sudo make install
```

# 基础模块开发
![build install gif](doc/pics/create_base_mod_proj.gif)

## 创建自定义基础模块工程
```py
python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --type="mod" --dir="/path/to/base_mode_dir/" --name="user_define_name"
```

## 自定义模块构建和安装
```sh
mkdir build
cd build
cmake ../
make
make install
```

## 基础模块开发目录组织结构
```
|--conf
|  |--hello.json
|--proto
|  |--hello.proto
|--com
|  |--hello.cpp
|--CMakeLists.txt
```

## 自定义模块开发三步走
### (一)开发模块需要定义的函数
```c++ 
// 创建变量对象函数
std::shared_ptr<::google::protobuf::Message> 
create_msg(const std::string& msg_name); 

//// 模块定义函数
// 一个输入输出参数定义
std::shared_ptr<::google::protobuf::Message> 
fun1(std::shared_ptr<::google::protobuf::Message>);
// 多个输入输出参数定义
std::vector<std::shared_ptr<::google::protobuf::Message>> 
fun2(std::vector<std::shared_ptr<::google::protobuf::Message>>);
```
### (二)开发模块需要定义的配置文件
> 注1: "_comment" 为注释行, 实际没有该字段  
> 注2: 关键字都以下划线开头
```json
{
    "_comment":"库模块名称",
    "_lib":"libbpmath.so",
    "_comment":"定义函数",
    "_func":{
        "_comment":"定义函数名add",
        "add":{
            "_comment":"定义函数输入参数",
            "_input":[
                {
                    "type":"bpmath.BpIntPair",
                    "name":"a and b"
                }
            ],
            "_comment":"定义函数输出参数",
            "_output":[
                {
                    "type":"bpbase.Int",
                    "name":"c"
                }
            ]
        }
    },
    "_comment":"该模块定义的消息",
    "_val":[
        {
            "type":"bpmath.BpIntPair",
            "desc":""
        }
    ]
}
```
### (三)开发模块需要定义的proto文件
可以自定义proto消息文件, 也可以使用基础模块已经有的proto消息文件

# 软件制作
## 编辑/导出/运行
![edit software gif](doc/pics/make_software.gif)
```sh
# 打开编辑器
sudo LD_LIBRARY_PATH=/opt/SoftwareFactory/lib /opt/SoftwareFactory/bin/SoftwareFactoryEditor
# 运行编辑好的程序
sudo LD_LIBRARY_PATH=/opt/SoftwareFactory/lib /opt/SoftwareFactory/bin/bptemplate -graph_exec_file="/path/to/hello_exec.json"
```

# 界面插件开发
## 创建界面插件工程
```py
python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --type="mod_panel" --dir="/path/to/panel_dir/" --name="user_panel_name"
```

## 界面插件构建和安装
```sh
mkdir build
cd build
cmake ../
make
make install
```

## 界面插件开发目录组织结构
```
|--panel_hello.json
|--panel_hello.cpp
|--CMakeLists.txt
```

## 界面插件开发两步走
### (一)重写panel函数
```c++ 
class panel_hello : public SFEPanel {
public:
    virtual bool Init() override {
        return true;
    }
    virtual void Update() override {
        ImGui::Begin(PanelName().c_str());

        ImGui::Button("Hello");
        
        ImGui::End();
    }
    virtual void Exit() override {

    }
    virtual void OnMessage(const SFEMessage& msg) override {

    }
};
```
### (二)修改界面插件配置文件
> 注1: "_comment" 为注释行, 实际没有该字段  
> 注2: 关键字都以下划线开头
```json
{
    "_comment":"库模块名称",
    "_lib":"libpanel_hello.so",
    "_comment":"定义插件实例",
    "_instance":[
        {
            "_comment":"实例1",
            "_comment":"定义panel对象名",
            "type":"panel_hello",
            "_comment":"定义panel实例名",
            "name":"panel_hello"
        }
    ]
}
```

# Useful example
> 基础模块工程例子：[https://github.com/lkpworkspace/bpio](https://github.com/lkpworkspace/bpio)

