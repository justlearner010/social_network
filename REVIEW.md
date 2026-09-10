# 评审记录（REVIEW）

> **声明**：本项目为《数据结构》课程项目。全部代码由 **AI 实现**，由作者（课程项目负责人）**人工 review 后合入**。
> 评审对象：每个功能 Issue 对应的 worktree 分支 `feat/<N>-<name>`；评审通过并以 merge commit 合入 `main` 后，该 Issue 关闭。

## 评审方式

每个分支合入前，作者按以下清单逐项 review：

- [ ] 接口与 DESIGN.md 契约一致（不擅改公共头文件）
- [ ] 数据结构实现正确（邻接矩阵对称性、邻接表构建/释放、顺序表增删）
- [ ] 算法正确且复杂度符合 DESIGN.md 约定（DFS/BFS O(V+E)、Prim O(V²)、Kruskal O(E log E)、Floyd O(V³)、Dijkstra O(V²)）
- [ ] 边界情况处理（不连通图、K 越界、重名/重编号、文件格式错误、资源释放）
- [ ] 本分支单元测试通过（`make test`）
- [ ] 无内存问题（`-fsanitize=address,undefined` 与 valgrind 通过）
- [ ] 仓库最终状态 GitHub CI 全绿

## 按分支评审记录

| Issue | 分支 | 内容 | 评审 | 合入 commit |
| --- | --- | --- | --- | --- |
| #1 | feat/01-storage | 系统输入与数据文件存储（storage.c + gen_data + data 样例） | ✅ 通过 | _merge commit_ |
| #2 | feat/02-user | 用户管理（user.c） | ✅ 通过 | _merge commit_ |
| #3 | feat/03-graph | 好友关系管理（graph.c + 邻接表） | ✅ 通过 | _merge commit_ |
| #4 | feat/04-traversal | 图遍历：DFS/BFS/距离 K（traversal.c） | ✅ 通过 | _merge commit_ |
| #5 | feat/05-mst | 最小生成树 Prim/Kruskal（mst.c） | ✅ 通过 | _merge commit_ |
| #6 | feat/06-floyd | 最短路径 Floyd（floyd.c） | ✅ 通过 | _merge commit_ |
| #7 | feat/07-dijkstra | 最短路径 Dijkstra（dijkstra.c） | ✅ 通过 | _merge commit_ |
| #8 | feat/08-ui | 交互菜单与关系图显示（ui.c + main.c） | ✅ 通过 | _merge commit_ |
| #9 | feat/09-examples | 使用样例与端到端测试 | ✅ 通过 | _merge commit_ |
| #10 | feat/10-ci | GitHub CI 工作流 | ✅ 通过 | _merge commit_ |

## 已知说明

- 密码为课程演示用途，明文存储，无安全要求。
- 亲密度权值由随机函数生成并固化在 `data/*.txt` 中（可复现）；运行期添加好友支持随机权值。