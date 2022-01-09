## 安装依赖
sudo apt-get install libsdl2-dev

## 面板
### 菜单栏面板
文件
    新建蓝图
        模块
        程序
    导入蓝图

### 模块库面板
### editor面板
### 蓝图变量面板
### 蓝图节点面板

## 尝试使用ECS开发界面
### system
* 所有面板算是system
    * 只提供显示方法
* 组件
    * 只存储状态,没有方法

* 蓝图显示系统 --> 蓝图组件
* 蓝图底层系统 --> 蓝图组件
* 输入系统 --> 输入组件(单例)


editor编辑数据
    * 
BpCore组织数据(Bp.h)
    * BpGraph
    * BpModLib(单例)
    * BpNodeLib(单例)