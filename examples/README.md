# 使用样例

本目录提供系统功能的使用样例，便于演示与验收（需求文档"任务要求：完成简单界面与菜单交互、完成功能需求"）。

## 运行方式

```bash
make            # 构建全部
./build/social_network data/users8.txt   # 8 用户：需求文档表 1 的示例
./build/social_network data/users20.txt  # 20 用户：满足"用户数 > 10"
```

## 样例文件

| 文件 | 说明 |
| --- | --- |
| `session8.txt` | 8 用户（需求文档示例）交互会话：展示用户、关系图、DFS/BFS、距离 K、Prim/Kruskal、Floyd/Dijkstra 最短路径（AlexWhite → AvaJones） |
| `session8.out.txt` | 上述会话的完整输出转录（运行 `./build/social_network data/users8.txt < examples/session8.txt` 得到） |
| `session20.txt` | 20 用户交互会话：遍历、最短路径、好友增删、保存数据 |
| `session20.out.txt` | 上述会话的完整输出转录 |
| `demo.sh` | 自动演示脚本：逐项驱动程序并校验输出关键标记（`make demo` 或 `bash examples/demo.sh`） |

## 会话内容（session8.txt）

```text
1               显示所有用户
2               显示关系图（邻接矩阵 + 边列表）
5 AlexWhite     DFS 遍历（显式栈，从 AlexWhite 出发）
6 AlexWhite     DFS 遍历（递归）
7 AlexWhite     BFS 遍历
8 AlexWhite 1   BFS 距离 K=1 的用户
9               Prim 最小生成树
10              Kruskal 最小生成树
11 AlexWhite AvaJones   Floyd 最短路径
12 AlexWhite AvaJones   Dijkstra 最短路径
0               退出
```

## 自动校验

```bash
make demo
# 输出形如：
#   PASS: 显示所有用户
#   PASS: 显示关系图(矩阵)
#   ...
#   [demo] 全部样例通过 ✔
```