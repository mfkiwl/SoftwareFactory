# 功能概述
该工具库只是充当胶水的作用将各个模块组织起来生成一个更大的工具程序.  
工具库一共分为两部分: 一个是运行工具的核心库以及包含的多个基础模块; 另外一个是图编辑程序,主要用于编辑更高级的模块组件(或者可执行程序).

# 开发目录组织结构
```
itools
|--3rd
|--core
|--common
|--conf
|  |--bpmath.json
|--proto
|  |--bpmath.proto
|--com
|  |--math
|  |  |--bpmath.hpp
|  |  |--bpmath.cpp
|  |  |--CMakeLists.txt
```
# 框架关键名词
## 核心库以及组件名词
### 基础模块
* 基础模块间的依赖关系
    * 基础模块间不能相互依赖
    * 基础模块的proto可以相互依赖
* 组织模块配置文件
    * 目前先实现一个个组织模块或者可执行程序,暂时不进行嵌套
* 组织模块间的依赖关系(多个基础模块组成)
    * 不能形成相互依赖
### 变量节点(val)
* 属性设置
    * 在编辑界面设置变量值, 用于支持UI界面设置默认值
### 分支控制节点(node)
* 用于控制程序流向节点
### 模块节点(node)
* 包含基础模块
### 图节点(graph)
* 包含组织模块
### 连线(link)
* 节点之间的连线

## 图编辑程序(UI)名词
* TODO

# 公共函数
```c++
// 设置变量属性
bool set_attr(std::shared_ptr<::google::protobuf::Message> msg, const std::string& json_str);
// 获得变量属性
bool get_attr(const std::shared_ptr<::google::protobuf::Message> msg, std::string& json_str);
```

# 自定义模块开发
## 开发模块需要定义的函数
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
## 开发模块需要定义的配置文件
> 注1: "_comment" 为注释行, 实际没有该字段  
> 注2: 关键字都以下划线开头
```json
{
    "_comment":"定义包名为bp",
    "np":{
        "_comment":"定义包名为math",
        "math":{
            "_comment":"定义包含的函数",
            "_func":{
                "_comment":"定义函数名add",
                "add":{
                    "_comment":"定义函数输入参数",
                    "_input":[
                        "bp.math.BpIntPair"
                    ],
                    "_comment":"定义函数输出参数",
                    "_output":[
                        "bp.base.BpInt"
                    ]
                }
            },
            "_comment":"定义该模块包含的消息",
            "_val":[
                "bp.base.BpIntPair"
            ]
        }
    }
}
```
## 开发模块需要定义的proto文件
可以自定义proto消息文件, 也可以使用基础模块已经有的proto消息文件
