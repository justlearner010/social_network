# 评审记录（REVIEW）

> **声明**：本项目为《数据结构》课程项目。全部代码由 **AI 实现**，由作者（课程项目负责人）**人工 review 后合入**。
> 评审对象：每个功能 Issue 对应的 worktree 分支 `feat/<N>-<name>`；评审通过并以 merge commit 合入 `main` 后，该 Issue 关闭。

## 评审方式

每个分支合入前，作者按以下清单逐项 review：

- [x] 接口与 DESIGN.md 契约一致（不擅改公共头文件）
- [x] 数据结构实现正确（邻接矩阵对称性、邻接表构建/释放、顺序表增删）
- [x] 算法正确且复杂度符合 DESIGN.md 约定（DFS/BFS O(V+E)、Prim O(V²)、Kruskal O(E log E)、Floyd O(V³)、Dijkstra O(V²)）
- [x] 边界情况处理（不连通图、K 越界、重名/重编号、文件格式错误、资源释放）
- [x] 本分支单元测试通过（`make test`）
- [x] 无内存问题（`-fsanitize=address,undefined` 与 valgrind 通过）
- [x] 仓库最终状态 GitHub CI 全绿

## 按分支评审记录

| Issue | 分支 | 内容 | 评审 | 合入 commit |
| --- | --- | --- | --- | --- |
| #3 | feat/03-storage | 系统输入与数据文件存储（storage.c + gen_data + data 样例） | ✅ 通过 | `2bc9af6` |
| #2 | feat/02-user | 用户管理（user.c） | ✅ 通过 | `0a0fb2a` |
| #4 | feat/04-graph | 好友关系管理（graph.c + 邻接表） | ✅ 通过 | `6f4b29b` |
| #1 | feat/01-traversal | 图遍历：DFS/BFS/距离 K（traversal.c） | ✅ 通过 | `94f63c2` |
| #7 | feat/07-mst | 最小生成树 Prim/Kruskal（mst.c） | ✅ 通过 | `f8b6479` |
| #8 | feat/08-floyd | 最短路径 Floyd（floyd.c） | ✅ 通过 | `e1e0419` |
| #5 | feat/05-dijkstra | 最短路径 Dijkstra（dijkstra.c） | ✅ 通过 | `4457068` |
| #6 | feat/06-ui | 交互菜单与关系图显示（ui.c + main.c） | ✅ 通过 | `c657e79` |
| #9 | feat/09-examples | 使用样例与端到端测试 | ✅ 通过 | `4a5f119` |
| #10 | feat/10-ci | GitHub CI 工作流 | ✅ 通过 | `9708bb8` |

## 合入后集成验证

- 全部单元/集成测试：**2513 项断言通过**（`make test`）
- 样例演示：`examples/demo.sh` 11 项 PASS
- 会话转录：`examples/session8.out.txt`（8 用户，需求文档示例）、`examples/session20.out.txt`（20 用户）
- 消毒器：ASan/UBSan 下测试全绿（CI 同款配置）；valgrind 由 CI 在 Linux 上执行

## 已知说明

- 密码为课程演示用途，明文存储，无安全要求。
- 亲密度权值由随机函数生成并固化在 `data/*.txt` 中（可复现）；运行期添加好友支持随机权值。
- 非连通图下：Prim 返回可达部分生成树，Kruskal 返回生成森林（DESIGN.md §6.4）。