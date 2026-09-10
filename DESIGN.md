# 小型社交网络系统 — 设计方案（SPEC）

> 项目需求文档：《docs/小型社交网络系统设计与实现.docx》（《数据结构》课程项目）
> 本文档是开发前的设计方案规格说明：定义数据结构、项目结构、接口与算法，作为后续按功能拆分 Issue、并行 worktree 开发时的统一契约。

---

## 1. 需求概述

依据需求文档整理的功能需求：

| 编号 | 功能域 | 需求 |
| --- | --- | --- |
| F1 | 系统输入 | 输入用户规模（>10 个）、用户基本信息（编号/姓名/密码/年龄），以及对称的亲密度关联矩阵 |
| F2 | 数据持久化 | 所有用户数据存储在文件中；初始信息从文件加载 |
| F3 | 用户管理 | 记录用户规模与用户基本信息（增/删/查/列） |
| F4 | 好友关系管理 | 添加好友、删除好友；图节点=用户，边=好友关系，权值=亲密度 |
| F5 | 图遍历应用 | DFS / BFS 遍历好友关系网络；给定 K 值，BFS 找出与某用户距离为 K 的所有用户 |
| F6 | 最小生成树应用 | Prim 或 Kruskal 求 MST（使所有用户连通且亲密度权值和最小），并输出 MST |
| F7 | 最短路径应用 | 弗洛伊德（Floyd）求任意两用户最短路径；迪杰斯特拉（Dijkstra）按用户名求两用户最短路径及长度（均以权值之和最小为标准） |
| F8 | 界面 | 简单界面显示关系图 + 交互菜单 |

**关键约定**（来自文档"任务说明"）：

1. 用户数大于 10 个。
2. 初始用户信息存储在文件中。
3. 构建**非稀疏的连通图**，节点间权值用随机函数生成，取值 `(0,1)`。
4. **权值越小，亲密度越高**；权值 `0` 表示无边。
5. 亲密度是相互的 → 图是**无向图**，关联矩阵为对称矩阵。

---

## 2. 总体架构

```
                    ┌─────────────────────────────────────────┐
                    │              UI 层（交互菜单）            │
                    │   src/ui.c + src/main.c                 │
                    └───────────────────┬─────────────────────┘
                                        │ 函数调用
        ┌───────────────┬───────────────┼───────────────────┬────────────┐
        ▼               ▼               ▼                   ▼            ▼
   █ 用户管理       █ 好友关系      █ 图遍历           █ 最小生成树   █ 最短路径
   src/user.c      src/graph.c    src/traversal.c     src/mst.c    src/floyd.c
                                                                    src/dijkstra.c
        │               │               │                   │            │
        └───────────────┴───────────────┴───────────────────┴────────────┘
                                        │
                    ┌───────────────────▼─────────────────────┐
                    │           数据层（文件持久化）             │
                    │   src/storage.c + data/*.txt             │
                    └─────────────────────────────────────────┘
```

- **数据层**：文本文件保存用户信息 + 关联矩阵；`storage_load / storage_save` 负责读写；`tools/gen_data.c` 负责按随机函数生成"非稀疏连通图"初始数据。
- **算法层**：图遍历（DFS/BFS/距离 K）、MST（Prim/Kruskal）、最短路径（Floyd/Dijkstra），全部工作在邻接矩阵/邻接表之上。
- **表示层**：菜单驱动的命令行交互，直接显示邻接矩阵与边列表作为"关系图"。

---

## 3. 项目结构

```
social_network/
├── README.md                 # 项目说明（课程项目声明、构建与使用）
├── DESIGN.md                 # 本设计方案（SPEC）
├── REVIEW.md                 # 评审记录（AI 实现，作者逐项 review）
├── Makefile                  # 构建脚本（app / test / demo / clean）
├── .github/workflows/ci.yml  # GitHub CI（feature: CI 工作流）
├── docs/
│   └── 小型社交网络系统设计与实现.docx   # 课程需求文档
├── include/
│   ├── user.h                # User / UserDB 结构体与用户管理接口
│   ├── graph.h               # Graph 邻接矩阵 + 邻接表结构体与好友管理接口
│   ├── traversal.h           # DFS/BFS/距离K 遍历接口
│   ├── mst.h                 # Prim/Kruskal 最小生成树接口
│   ├── shortest.h            # Floyd/Dijkstra 最短路径接口
│   ├── storage.h             # 文件读写接口
│   └── ui.h                  # 交互菜单接口
├── src/
│   ├── user.c                # 用户管理实现
│   ├── graph.c               # 好友关系管理实现 + 邻接表构建
│   ├── traversal.c           # DFS（递归/显式栈）、BFS、BFS 距离 K
│   ├── mst.c                 # Prim（O(V²)）、Kruskal（并查集）
│   ├── floyd.c               # Floyd-Warshall 全源最短路径
│   ├── dijkstra.c            # Dijkstra 单源最短路径
│   ├── storage.c             # 文件加载/保存
│   ├── ui.c                  # 菜单交互
│   └── main.c                # 程序入口
├── tools/
│   └── gen_data.c            # 随机生成初始数据文件（连通非稀疏图）
├── data/
│   ├── users8.txt            # 需求文档示例（表1，8 个用户）
│   └── users20.txt           # 20 个用户示例（默认数据）
├── tests/
│   ├── test_util.h           # 轻量断言工具
│   ├── test_user.c           # 用户管理测试
│   ├── test_storage.c        # 文件读写测试
│   ├── test_graph.c          # 好友关系管理测试
│   ├── test_traversal.c      # DFS/BFS/距离K 测试
│   ├── test_mst.c            # Prim/Kruskal 测试
│   ├── test_shortest_floyd.c # Floyd 测试
│   ├── test_shortest_dijkstra.c # Dijkstra 测试
│   └── test_e2e.c            # 端到端集成测试（真实数据文件）
└── examples/
    ├── README.md             # 使用样例说明
    ├── demo.sh               # 自动演示脚本（喂入交互会话并校验输出）
    ├── session8.txt          # 8 用户示例会话（需求文档示例）
    └── session20.txt         # 20 用户示例会话
```

---

## 4. 数据结构设计

### 4.1 用户结构体（需求文档参考实现 → 本实现）

需求文档参考：

```c
typedef struct User {
    char ID[10];
    char username[20];
    char password[20];
    int age;
} User;
```

本实现（保持字段一致，长度改为宏常量）：

```c
#define MAX_USERS     100   /* 社交网络中最大用户数（同文档参考） */
#define USER_ID_LEN   10
#define USER_NAME_LEN 20
#define USER_PASS_LEN 20

typedef struct User {
    char id[USER_ID_LEN];       /* 编号，如 20250621 */
    char username[USER_NAME_LEN]; /* 姓名 */
    char password[USER_PASS_LEN]; /* 密码 */
    int  age;                    /* 年龄 */
} User;

typedef struct UserDB {         /* 用户库：顺序表存储 */
    User users[MAX_USERS];
    int  count;                 /* 当前用户规模 */
} UserDB;
```

**实现方式**：`UserDB` 采用**顺序存储结构（数组）**。原因：用户总量受 `MAX_USERS` 上界约束、随机访问频率高（按编号/姓名查下标）、规模小，数组的 O(1) 随机访问优于链表，且便于直接映射到邻接矩阵下标。删除用户采用"尾元素覆盖 + count--"的 O(1) 删除。

### 4.2 图的邻接矩阵表示（需求文档参考 → 本实现）

需求文档参考：

```c
#define MAX_USERS 100
typedef struct Graph {
    float adjMatrix[MAX_USERS][MAX_USERS];
    int numUsers;
} Graph;
```

本实现：

```c
#define EDGE_NONE 0.0f   /* 0.0 表示无边；权值在 (0,1)，越小亲密度越高 */
#define GRAPH_INF 1e9f   /* 算法中用到的"无穷大" */

typedef struct Graph {
    float adj[MAX_USERS][MAX_USERS]; /* 亲密度矩阵（对称） */
    int   num_users;
} Graph;
```

**实现方式**：`float adj[MAX_USERS][MAX_USERS]` 对称矩阵，`adj[u][v] == 0.0f` 表示无边。此结构是 MST（Prim）与最短路径（Floyd/Dijkstra）的直接载体，O(1) 判边/取权值。

### 4.3 图的邻接表表示（需求文档参考 → 本实现，用于图遍历）

需求文档参考：

```c
typedef struct Edge      { int to; float weight; struct Edge *next; } Edge;
typedef struct AdjListNode { User user; Edge *head; } AdjListNode;
typedef struct AdjList   { AdjListNode list[MAX_USERS]; int numUsers; } AdjList;
```

本实现（结构一致，命名微调）：

```c
typedef struct Edge {           /* 邻接表的一条边 */
    int to;                     /* 指向朋友节点的下标 */
    float weight;               /* 亲密度权值 */
    struct Edge *next;
} Edge;

typedef struct AdjNode {        /* 顶点节点 */
    User user;                  /* 用户信息副本 */
    Edge *head;                 /* 邻接链表头 */
} AdjNode;

typedef struct AdjList {
    AdjNode nodes[MAX_USERS];
    int num_users;
} AdjList;
```

**实现方式**：`AdjList` 由 `adjlist_build()` 从邻接矩阵**一次性构建**（头插法；构建时按 v 降序插入，保证链表内邻居按 v 升序，使 DFS/BFS 输出确定可测），用后由 `adjlist_free()` 释放。邻接表专门服务于图遍历（DFS/BFS/距离 K），避免遍历时 O(V²) 扫描矩阵。

### 4.4 算法辅助结构

```c
/* 遍历结果：访问顺序 */
typedef struct VisitResult { int order[MAX_USERS]; int len; } VisitResult;

/* BFS 距离 K 结果：与起点距离恰为 K 的用户集合 */
typedef struct DistResult  { int users[MAX_USERS]; int len; } DistResult;

/* MST 结果 */
typedef struct MstEdge     { int u, v; float weight; } MstEdge;
typedef struct MstResult   { MstEdge edges[MAX_USERS-1]; int edge_count; float total_weight; } MstResult;

/* 最短路径结果 */
typedef struct PathResult  { int nodes[MAX_USERS]; int len; float dist; } PathResult;
/* nodes[]: 含首尾的路径节点序列；len: 节点个数(0=不可达)；dist: 路径权值和 */
```

---

## 5. 文件存储格式（data/*.txt）

文本文件，UTF-8，三部分：

```
<用户数 N>                                  # 第 1 行
<编号> <姓名> <密码> <年龄>                 # 第 2..N+1 行
<……共 N 行用户信息……>
<w00 w01 … w0(N-1)>                         # 第 N+2..2N+1 行
<……共 N 行 × N 列浮点数，构成对称矩阵……>   # 0.000 表示无边，(0,1) 表示亲密度
```

例（data/users8.txt 头部）：

```
8
20250621 AlexWhite a1B2c3D4 25
20250622 EmmaClark x9Y8z7W6 30
...
0.000 0.137 0.422 0.000 ...
...
```

- 由 `tools/gen_data.c` 生成：随机组合姓名、密码、年龄，权值用随机函数生成 `(1~999)/1000 ∈ (0,1)`；先以 ~65% 密度随机铺边，再用 BFS 校验连通性，若不连通则补充随机边，**保证文件内即为"非稀疏连通图"**。
- `data/users8.txt` 的用户信息严格取自需求文档"表 1"（8 个用户）；`data/users20.txt` 为 20 个用户（满足"用户数大于 10"）。

---

## 6. 算法设计

> 权值语义：整数路径/树的"代价" = 路径上**权值之和**，越小的代价对应越高的总体亲密度。以下所有算法都以权值之和最小为目标（与需求一致）。

### 6.1 深度优先搜索（DFS）— O(V+E)

- `traversal_dfs`：**显式栈**实现（数组模拟栈，避免递归栈溢出，可展示栈的应用）。
- `traversal_dfs_recursive`：经典递归实现。
- 两者基于邻接表遍历，输出访问顺序 `VisitResult`。
- 伪代码（显式栈版）：

```
stack <- [start]
while stack 非空:
    u <- stack.pop()
    if visited[u]: continue
    visited[u] <- true; order.append(u)
    for v in al.nodes[u] 的邻居（逆序入栈，保证按升序访问）:
        if !visited[v]: stack.push(v)
```

### 6.2 广度优先搜索（BFS）— O(V+E)

- `traversal_bfs`：**循环队列**（数组实现）遍历，输出访问顺序。
- 伪代码：

```
queue.enqueue(start); visited[start] <- true; order.append(start)
while queue 非空:
    u <- queue.dequeue()
    for v in al.nodes[u] 的邻居（升序）:
        if !visited[v]: visited[v] <- true; order.append(v); queue.enqueue(v)
```

### 6.3 BFS 距离 K — O(V+E)

- `traversal_bfs_distance_k(al, start, k)`：一次 BFS 记录每个节点的跳数距离 `dist[]`（`dist[start]=0, dist[v]=dist[u]+1`），返回所有 `dist[v] == k` 的节点集合。**距离定义 = 好友链上的跳数**（需求："与某个用户距离为 k 的所有用户"）。
- 当 `k < 0` 或 `k > 图直径` 时返回空集。

### 6.4 最小生成树 — Prim（O(V²)）与 Kruskal（O(E log E)）

需求：以最小"代价"将所有用户连成紧密群体，输出 MST。

**Prim（普里姆）** — `mst_prim`，基于邻接矩阵，适合稠密图：

```
dist[] <- INF; inTree[] <- false; parent[] <- -1
dist[0] <- 0   /* 任取起点，这里取 0 号用户 */
for i in 0..V-1:
    u <- 未入树中 dist 最小的顶点（dist=INF 则说明图不连通，提前终止）
    inTree[u] <- true
    if u != 起点: 记录边 (parent[u], u)
    for v: if !inTree[v] 且 w(u,v)>0 且 w(u,v) < dist[v]:
        dist[v] <- w(u,v); parent[v] <- u
```

**Kruskal（克鲁斯卡尔）** — `mst_kruskal`，辅助"并查集（Union-Find）"：

```
将图中所有边 (u<v, w) 收集并按 w 升序 qsort
parent[] 并查集初始化（每个顶点自成一集）
for 每条边按序:
    if find(u) != find(v):        /* 不成环 */
        union(u, v)
        记录边；若已收集 V-1 条 → 结束
```

- 比较与校验：连通图下两者 `edge_count == V-1`，且**总权值必然相等**（MST 权值和唯一），测试中以该性质互验；图不连通时 Prim 返回可达部分生成树、Kruskal 返回生成森林（各自提前终止）。

### 6.5 最短路径 — 弗洛伊德（Floyd，O(V³)）

需求：输出**任意两用户**之间的最短路径，采用弗洛伊德算法。

- `shortest_floyd(g, dist[][], next[][])`：动态规划思想，`dist[i][j]` 为当前最短权值和，`next[i][j]` 记录路径上 i 的下一个节点用于重构。

```
dist[i][j] = w(i,j)（无边则 INF）；next[i][j] = j（无边则 -1）；dist[i][i]=0, next[i][i]=i
for k in 0..V-1:
    for i in 0..V-1:
        for j in 0..V-1:
            if dist[i][k] + dist[k][j] < dist[i][j]:
                dist[i][j] <- dist[i][k] + dist[k][j]
                next[i][j] <- next[i][k]
```

- `shortest_floyd_path(g, s, t)`：按 `next` 链重建路径。

### 6.6 最短路径 — 迪杰斯特拉（Dijkstra，O(V²)）

需求：输入两个用户名，返回两用户间最短路径及长度。

- `shortest_dijkstra(g, s, t)`：经典教材版 O(V²)（数组选最小未定节点，无堆），适配课程深度；图不连通时对不可达对返回 `len=0`。

```
dist[] <- INF; visited[] <- false; prev[] <- -1; dist[s] <- 0
for i in 0..V-1:
    u <- 未访问中 dist 最小者（dist=INF → 剩余不可达，终止）
    visited[u] <- true
    for v: if !visited[v] 且 w(u,v)>0 且 dist[u]+w(u,v) < dist[v]:
        dist[v] <- dist[u]+w; prev[v] <- u
从 t 沿 prev 回溯重建路径；权值和 = dist[t]
```

- 正确性前提：权值均为正（`(0,1)`），满足 Dijkstra 使用条件。

---

## 7. 模块接口（头文件契约）

接口固定（本 spec 定义），各功能分支只实现对应 `.c`，不互相改动头文件，从而支持并行 worktree 无冲突开发：

| 头文件 | 主要接口 |
| --- | --- |
| `include/user.h` | `userdb_init / userdb_add / userdb_remove / userdb_find_by_id / userdb_find_by_name / userdb_get / userdb_print` |
| `include/graph.h` | `graph_init / graph_set_edge / graph_del_edge / graph_has_edge / graph_weight / graph_is_connected / graph_print_edges / graph_print_matrix / adjlist_build / adjlist_free` |
| `include/traversal.h` | `traversal_dfs / traversal_dfs_recursive / traversal_bfs / traversal_bfs_distance_k / visit_print / dist_print` |
| `include/mst.h` | `mst_prim / mst_kruskal / mst_print` |
| `include/shortest.h` | `shortest_floyd / shortest_floyd_path / shortest_dijkstra / path_print` |
| `include/storage.h` | `storage_load / storage_save` |
| `include/ui.h` | `ui_run` |

---

## 8. 构建、测试与持续集成

- 构建：`make`（默认 `all`：app + tests）；单项：`make app`、`make test`、`make demo`、`make clean`。
- 测试框架：`tests/test_util.h` 提供 `CHECK/CHECK_CLOSE/test_summary` 轻量断言；`make test` 自动编译运行 `tests/test_*.c`（每个测试为独立可执行文件，链接除 `main.o` 外的全部模块）。
- CI（`.github/workflows/ci.yml`）：
  1. `build`：gcc 严格告警（`-Wall -Wextra -Wpedantic -Werror`）编译 + `make test` + （模块齐全后）`make app`；
  2. `sanitize`：`-fsanitize=address,undefined` 重跑测试，检测内存/未定义行为；对可执行 app 用 valgrind 校验无泄漏；
  3. `demo`：运行 `examples/demo.sh` 自动演示与校验样例输出。

---

## 9. 边界与假设

1. `MAX_USERS = 100`；文件中的用户数 `N ∈ [1, 100]`，需求要求 N > 10（样例满足）。
2. 权值 ∈ `(0,1)`，`0.0` 表示无边；对称矩阵保证无向性；小权值 = 高亲密度。
3. 算法对"非连通图"均有容错：MST 提前终止并以 `edge_count < V-1` 标识；Floyd/Dijkstra 对不可达对返回空路径（`len=0`）。
4. 首次邻接表由 `adjlist_build` 从矩阵构建，图变更后需重新构建。
5. 用户编号、姓名全局唯一（`userdb_add` 校验）；编号/姓名/密码长度分别受 `USER_ID_LEN/USER_NAME_LEN/USER_PASS_LEN` 限制。
6. 密码仅作课程演示，明文存储，无安全要求。