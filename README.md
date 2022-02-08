# 软件工厂

# 功能概述
该工具库只是充当胶水的作用将各个模块组织起来生成一个更大的工具程序.  
工具库一共分为两部分:  
1. 运行软件的核心库以及多个基础模块; 
2. 软件工厂编辑器,主要用于编辑更高级的模块组件(或者可执行程序).

# 开发/运行环境
操作系统: Ubuntu 20.04

# 安装依赖
```sh
sudo apt-get install build-essential
sudo apt-get install libjsoncpp-dev libprotobuf-dev protobuf-compiler libgflags-dev libgtest-dev libgoogle-glog-dev 
sudo apt-get install libsdl2-dev libglew-dev libgtk-3-dev
```

# 构建
```sh
mkdir build
cd build
cmake ../
make
sudo make install
```

# 可视化软件制作
## 编辑
```sh
# 打开编辑器
sudo LD_LIBRARY_PATH=/opt/SoftwareFactory/lib ./SoftwareFactoryEditor
```

## 导出
Save as... --> xxx.json

## 运行
```sh
sudo LD_LIBRARY_PATH=/opt/SoftwareFactory/lib ./bptemplate -graph_exec_file="xxx.json"
```

# 基础模块开发
## 创建自定义基础模块工程
```py
# TODO
python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --dir=/home/xxx/base_mode_dir --name="user_define_name"
```

## 基础模块开发目录组织结构
```
|--conf
|  |--bpmath.json
|--proto
|  |--bpmath.proto
|--com
|  |--bpmath.hpp
|  |--bpmath.cpp
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
fun(std::shared_ptr<::google::protobuf::Message>);
// 多个输入输出参数定义
std::vector<std::shared_ptr<::google::protobuf::Message>> 
fun(std::vector<std::shared_ptr<::google::protobuf::Message>>);
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
                "bpmath.BpIntPair"
            ],
            "_comment":"定义函数输出参数",
            "_output":[
                "bpbase.Int"
            ]
        }
    },
    "_comment":"该模块定义的消息",
    "_val":[
        "bpmath.BpIntPair"
    ]
}
```
### (三)开发模块需要定义的proto文件
可以自定义proto消息文件, 也可以使用基础模块已经有的proto消息文件

## 自定义模块构建和安装
```sh
mkdir build
cd build
cmake ../
make
make install
```