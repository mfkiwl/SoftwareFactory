# 各个panel可以处理的命令

# [全局命令](#全局命令)
## 当前图运行状态
```json
{
    "command":"run_cur_graph",
    "type": [ "req" | "resp" ],
    "run": [ true | false ]
}
```
## 启动/继续/停止调试
```json
{
    "command":"debug_cur_graph",
    "type": [ "req" | "resp" ],
    "stage": [ "start" | "continue" | "stop" ]
}
```

# [SFEditor](#SFEditor)
## 创建图
```json
{
    "command":"create_new",
    "graph_name": "test",
    "graph_type": "mod graph"
}
```
## 创建事件节点
```json
{
    "command":"spawn_node",
    "type": 3,
    "node_name": "Begin"
}
```
## 加载图
```json
{
    "command":"open_graph",
    "path": "path/to/graph"
}
```
## 保存图
```json
{
    "command":"save_graph_step2",
    "path": "path/to/graph"
}
```
## 切换图
```json
{
    "command":"switch_graph",
    "name": "graph name"
}
```
## 删除节点
## 删除事件节点
## 创建连线
## 删除连线
## 创建pin
## 删除pin

# [SFEPanelBp](#SFEPanelBp)
## 设置单个节点位置
```json
{
    "command":"set_node_pos",
    "node_id":1,
    "x": 0,
    "y": 0
}
```
## 设置多个节点位置
```json
{
    "command":"set_nodes_pos",
    "desc":""
}
```
## 发送图节点位置给editor
```json
{
    "command":"save_graph_step1",
    "10":{
        "pos":[
            0,
            0
        ]
    },
    ...
}
```
## 将选中的节点移动到视图中央
```json
{
    "command":"move_node_to_center",
    "id":10
}
```

# [SFEPanelDragTip](#SFEPanelDragTip)
## 显示提示窗口
```json
{
    "command":"show",
    "desc":"some tip"
}
```
## 关闭提示窗口
```json
{
    "command":"close"
}
```

# [SFEPanelGraph](#SFEPanelGraph)
## 设置当前显示的图
```json
{
    "command":"set_cur_graph",
    "graph_name":"graph name"
}
```