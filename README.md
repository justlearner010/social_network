# 小型社交网络系统

[![CI](https://github.com/justlearner010/social_network/actions/workflows/ci.yml/badge.svg)](https://github.com/justlearner010/social_network/actions/workflows/ci.yml)

> **课程项目声明**：本仓库是《数据结构》课程的课程项目（"小型社交网络系统设计与实现"，需求见 [docs/小型社交网络系统设计与实现.docx](docs/小型社交网络系统设计与实现.docx)）。
> 本项目代码整体由 **AI 实现**，由作者（课程项目负责人）**逐功能分支 review 后合入**：每个功能对应一个 GitHub Issue、一个独立 worktree 分支，代码必须通过本地测试与 GitHub CI，并在合入前由作者人工评审（评审记录见 [REVIEW.md](REVIEW.md)）。

一个基于**图结构**的小型社交网络系统，用于实践《数据结构》课程中的图知识：

- 使用**邻接矩阵**存储用户好友关系（亲密度权值）
- 使用**邻接表**辅助实现遍历
- 实现 **DFS / BFS 遍历**、**BFS 距离 K 查询**、**Prim / Kruskal 最小生成树**、**Floyd / Dijkstra 最短路径**
- 数据存储于文件（`data/*.txt`），命令行菜单交互并显示关系图

设计规格见 [DESIGN.md](DESIGN.md)（数据结构、算法、存储格式、接口契约）。

## 功能

| 功能 | 说明 |
| --- | --- |
| 系统输入 | 从文件加载用户规模（>10）、用户信息（编号/姓名/密码/年龄）与对称亲密度矩阵 |
| 用户管理 | 记录并维护用户规模与基本信息（增/删/查/列） |
| 好友关系管理 | 添加好友、删除好友（图边增删，权值=亲密度） |
| 图遍历 | DFS（递归 + 显式栈）与 BFS 遍历好友网络 |
| BFS 距离 K | 找出与某用户距离恰好为 K 的所有用户 |
| 最小生成树 | Prim（O(V²)）与 Kruskal（并查集，O(E log E)），输出 MST 及总权值 |
| 最短路径 | Floyd 全源最短路径；Dijkstra 单源最短路径（按用户名输入） |
| 关系图显示 | 命令行显示邻接矩阵与边列表 |
| 数据持久化 | 初始化/保存/随机生成数据文件 |

## 快速开始

需要 C11 编译器（gcc / clang）与 make。

### 老师/助教一键运行（无需命令行）

| 平台 | 方式 |
| --- | --- |
| Windows | 双击 **run.bat**（自动编译 → 演示 / 交互 / 测试三选一菜单） |
| macOS | 双击 **run.command**（若提示无法打开：右键 → 打开） |
| Linux / 终端 | `./run.sh` |

```bash
# 构建
make

# 运行（默认加载 data/users20.txt，20 个用户）
./build/social_network

# 指定数据文件（如需求文档示例的 8 用户数据）
./build/social_network data/users8.txt

# 运行全部单元测试 / 端到端测试
make test

# 自动演示样例会话（examples/demo.sh）
make demo

# 用某个种子重新生成初始数据文件（连通非稀疏图）
make gen
./build/gen_data 20 42 data/users20.txt
```

运行后可用的菜单操作：显示用户、显示关系图、添加/删除好友、DFS/BFS 遍历、BFS 距离 K、Prim/Kruskal 最小生成树、Floyd/Dijkstra 最短路径、保存数据。

## 课程报告

课程设计报告（学号 202532110113 刘亦轩，含全部源码附录）：

- `docs/202532110113-刘亦轩-项目2-小型社交网络系统设计与实现.docx`（可直接提交/打印）
- 生成脚本 `docs/gen_report.py`（改内容后重跑：`python3 docs/gen_report.py`）
- 需求原始文档：`docs/小型社交网络系统设计与实现.docx`

## 使用样例

见 [examples/README.md](examples/README.md)：包含与需求文档示例（8 用户）对应的交互会话 [examples/session8.txt](examples/session8.txt)、20 用户会话 [examples/session20.txt](examples/session20.txt)，以及自动校验脚本 [examples/demo.sh](examples/demo.sh)。

```bash
# 8 用户示例会话（对应需求文档表 1）
./build/social_network data/users8.txt < examples/session8.txt
```

## 数据文件格式

```
<用户数 N>
<编号> <姓名> <密码> <年龄>        # 共 N 行
0.000 0.137 0.422 ...              # N×N 对称亲密度矩阵；0=无边，(0,1)=亲密度，越小越亲密
```

初始数据由 `tools/gen_data.c` 生成：确保图为**非稀疏连通图**、权值 ∈ (0,1)（越小亲密度越高）、矩阵对称。

## 项目结构

- `include/` — 公共头文件（数据结构 + 接口契约，由 DESIGN.md 定义）
- `src/` — 各功能模块实现（user / graph / traversal / mst / floyd / dijkstra / storage / ui / main）
- `tools/` — 初始数据生成工具
- `tests/` — 单元测试与端到端测试（`make test`）
- `examples/` — 使用样例（会话脚本 + 自动演示）
- `data/` — 示例数据文件
- `.github/workflows/ci.yml` — GitHub Actions 持续集成（构建 / 测试 / 消毒器 / 样例演示）

## 开发流程说明

按"一个功能一个 Issue、并行 worktree"开发：

1. 本仓库 `main` 先落定设计方案（DESIGN.md）与接口头文件；
2. 每个功能（需求文档 F1~F8 + 测试 + CI）一个 GitHub Issue；
3. 每个 Issue 在独立 worktree 分支 `feat/<N>-<name>` 上实现与自测；
4. 分支按依赖顺序合入 `main`，CI 全绿、作者 review 通过后关闭对应 Issue。

详见 [REVIEW.md](REVIEW.md)（评审记录）与 [DESIGN.md](DESIGN.md)（设计规格）。