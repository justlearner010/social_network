#!/usr/bin/env python3
"""生成《课程设计报告》docx —— 项目2：小型社交网络系统设计与实现
学号 202532110113 刘亦轩

样式参照 ezfilesystem/docx/202532110113-刘亦轩.docx 模板：
正文宋体小四，代码 Times New Roman 小五、行距固定值 10。

用法: python docs/gen_report.py   （输出到 docs/202532110113-刘亦轩-项目2.docx）
"""
import os
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_LINE_SPACING
from docx.oxml.ns import qn

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "docs", "202532110113-刘亦轩-项目2-小型社交网络系统设计与实现.docx")


def set_run(run, ascii_font="Times New Roman", east_font="宋体", size=12, bold=False, color=None):
    run.font.name = ascii_font
    run.font.size = Pt(size)
    run.font.bold = bold
    rPr = run._element.get_or_add_rPr()
    rFonts = rPr.find(qn("w:rFonts"))
    if rFonts is None:
        rFonts = rPr.makeelement(qn("w:rFonts"), {})
        rPr.append(rFonts)
    rFonts.set(qn("w:eastAsia"), east_font)
    if color:
        run.font.color.rgb = RGBColor(*color)


def para(doc, text, size=12, bold=False, align=None, east="宋体", ascii_="Times New Roman",
         line=1.5, space_before=0, space_after=6):
    p = doc.add_paragraph()
    if align is not None:
        p.alignment = align
    pf = p.paragraph_format
    pf.space_before = Pt(space_before)
    pf.space_after = Pt(space_after)
    if line == "exact10":
        pf.line_spacing_rule = WD_LINE_SPACING.EXACTLY
        pf.line_spacing = Pt(10)
    else:
        pf.line_spacing = line
    r = p.add_run(text)
    set_run(r, ascii_font=ascii_, east_font=east, size=size, bold=bold)
    return p


def heading(doc, text, level=1):
    sizes = {1: 14, 2: 12}
    return para(doc, text, size=sizes.get(level, 12), bold=True, east="黑体", ascii_="Arial",
                space_before=10, space_after=6)


def code(doc, text):
    """代码段：Times New Roman 小五（9pt），行距固定值 10"""
    for line in text.rstrip("\n").split("\n"):
        p = doc.add_paragraph()
        pf = p.paragraph_format
        pf.space_before = Pt(0)
        pf.space_after = Pt(0)
        pf.line_spacing_rule = WD_LINE_SPACING.EXACTLY
        pf.line_spacing = Pt(10)
        r = p.add_run(line)
        set_run(r, ascii_font="Courier New", east_font="宋体", size=9)


def read_file(rel):
    with open(os.path.join(ROOT, rel), encoding="utf-8") as f:
        return f.read()


def appendix(doc, files, header):
    para(doc, header, size=12, bold=True, east="黑体", space_before=8)
    for rel in files:
        fn = os.path.basename(rel)
        doc.add_paragraph()
        code(doc, "/****** %s ******/" % fn)
        code(doc, read_file(rel))
        doc.add_paragraph()


def main():
    doc = Document()
    # 默认样式：中文宋体
    style = doc.styles["Normal"]
    style.font.name = "Times New Roman"
    style.element.rPr.rFonts.set(qn("w:eastAsia"), "宋体")
    style.font.size = Pt(12)

    # ================= 标题 =================
    para(doc, "《数据结构》课程设计报告", size=18, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER,
         east="黑体", ascii_="Arial", space_after=4)
    para(doc, "项目2：小型社交网络系统设计与实现", size=16, bold=True,
         align=WD_ALIGN_PARAGRAPH.CENTER, east="黑体", ascii_="Arial", space_after=4)
    para(doc, "学号：202532110113        姓名：刘亦轩", size=12,
         align=WD_ALIGN_PARAGRAPH.CENTER, space_after=12)

    # ================= 1 项目说明 =================
    heading(doc, "1. 项目说明")
    for t in [
        "数字化时代，社交网络已成为人们生活中不可或缺的一部分。本项目运用《数据结构》课程中图结构的相关知识，设计并实现了一个小型社交网络系统：以用户为节点、以用户之间的好友关系为边，边的权值表示用户之间的亲密度（记为 0 到 1 之间的小数，数值越小代表亲密度越高，且亲密度相互对称）。系统在此基础上应用图遍历（DFS/BFS）、最小生成树（Prim/Kruskal）、最短路径（Floyd/Dijkstra）等算法，解决“遍历整个好友圈”“找出距离某用户 K 跳的所有朋友”“以最小亲密度代价把所有人连成一个紧密群体”“查询任意两人之间的最短好友链”等社交网络中的实际问题。",
        "技术实现方面：采用 C11 编写，数据以文本文件持久化（用户信息 + 对称亲密度矩阵，内存中以邻接矩阵为主存储、邻接表辅助遍历），提供命令行交互菜单并直接以邻接矩阵与边列表展示关系图；配套 8 组单元/集成测试与 GitHub Actions 持续集成（严格告警编译、ASan/UBSan 与 valgrind 内存检测、示例会话演示共三条流水线，全部为绿色）。",
        "开发模式：本项目代码整体由 AI 实现、由作者按“一个功能一个 Issue、并行 worktree 开发”的方式逐分支 review 后合入（详见第 5 节），保证合入代码经过人工评审与自动化验证。仓库：github.com/justlearner010/social_network。",
    ]:
        para(doc, t)

    # ================= 2 项目内容和要求 =================
    heading(doc, "2. 项目内容和要求")
    para(doc, "2.1 系统输入", size=12, bold=True)
    para(doc, "输入用户规模（如 20 个）和用户基本信息（编号、姓名、密码、年龄等），以及用户关联矩阵；矩阵中的实数值表示用户之间的亲密度（如聊天频率、共同兴趣爱好等）。亲密度是相互的，关联矩阵为对称矩阵。任务约定：用户数大于 10 个；初始用户信息存储在文件中；构建一个非稀疏的连通图，节点之间的权值使用随机函数生成（0，1）之间的值；图中弧上的权值越小，亲密度越高。")
    para(doc, "2.2 功能需求", size=12, bold=True)
    for t in [
        "（1）用户管理：记录用户的规模和用户基本信息（增、删、查、列）。",
        "（2）好友关系管理：用户可以添加好友、删除好友；系统用图表示好友关系，节点为用户，边表示好友关系，边上的权值表示亲密度。",
        "（3）图遍历应用：实现深度优先搜索（DFS）和广度优先搜索（BFS）算法遍历好友关系网络（即通过 DFS/BFS 构建网络中所有用户之间的关系）；给定合适的 K 值，利用 BFS 找出与某用户距离为 K 的所有用户。",
        "（4）最小生成树应用：以最小的代价将所有用户连接成一个紧密的群体，采用最小生成树算法（Prim 或 Kruskal）找出子图并输出最小生成树（亲密度权值之和最小）。",
        "（5）最短路径应用：采用弗洛伊德算法输出任意两个用户之间的最短路径（权值之和最小）；输入两个用户名，采用迪杰斯特拉算法返回这两个用户之间的最短路径及路径长度。",
        "（6）界面与持久化：设计简单的界面显示关系图，设计简单菜单便于交互；所有用户数据要求存储在文件中。",
    ]:
        para(doc, t)
    para(doc, "2.3 数据结构要求（参考，本项目按此实现）", size=12, bold=True)
    for t in [
        "用户结构体：用于存储用户的基本信息，如编号（ID）、姓名（username）、密码（password）、年龄（age）。",
        "图的邻接矩阵表示：使用二维数组表示用户之间的好友关系，元素为好友存在性与亲密度权值（float adjMatrix[MAX_USERS][MAX_USERS]，MAX_USERS=100）。",
        "图的邻接表表示（辅助图遍历）：每个用户节点对应一个链表，链表中存储该用户的所有好友节点及其亲密度权值（Edge{ to, weight, next } / AdjListNode{ user, head }）。",
    ]:
        para(doc, t)

    # ================= 3 关键算法的设计思路 =================
    heading(doc, "3. 关键算法的设计思路")
    para(doc, "3.1 总体结构与数据结构设计", size=12, bold=True)
    para(doc, "系统按单向依赖划分为五个模块：storage（文件读写：用户信息 + 对称亲密度矩阵）→ user（用户管理：顺序存储 UserDB）→ graph（好友关系管理：邻接矩阵 Graph + 邻接表 AdjList 构建/释放）→ 算法层（traversal 遍历 / mst 最小生成树 / floyd、dijkstra 最短路径）→ ui/main（菜单交互与程序入口）。算法层与数据层共用公共头文件定义的结构体（include/*.h 为契约，先于开发在 DESIGN.md 中定稿）。")
    para(doc, "内存表示上同时保留两种图结构：邻接矩阵 adj[MAX_USERS][MAX_USERS] 负责 O(1) 判边与取权值（MST/最短路径直接在其上计算）；邻接表由 adjlist_build 一次性构建（头插法且邻居按编号升序，输出确定可测），专供 DFS/BFS 遍历时避免 O(V²) 扫描。数据文件格式为：第 1 行用户数 N，随后 N 行“编号 姓名 密码 年龄”，再 N 行 × N 列浮点对称矩阵；tools/gen_data.c 用随机函数生成连通非稀疏图（约 65% 密度铺边 + BFS 校验连通并补边），权值取 (1~999)/1000，保证文件内即为合法初始数据。")
    para(doc, "3.2 图的遍历：DFS 与 BFS（O(V+E)）", size=12, bold=True)
    para(doc, "DFS 提供两种实现：经典递归版；显式栈版（数组模拟栈，弹栈时访问、逆序压入未访问邻居以保证访问顺序确定）。BFS 使用循环队列，入队即标记访问。两者均以访问顺序数组 VisitResult 返回。BFS 距离 K（traversal_bfs_distance_k）在一次 BFS 中记录每点跳数距离 dist[v]=dist[u]+1，返回所有 dist[v]==k 的用户集合；K 越界或为负时返回空集。")
    para(doc, "3.3 最小生成树：Prim（O(V²)）与 Kruskal（O(E log E)）", size=12, bold=True)
    para(doc, "Prim 基于邻接矩阵，适合稠密图：任选 0 号用户为根，维护未入树顶点的最小切分权值 dist[]，每轮取最小者入树并松弛，父指针记录树边。Kruskal 先收集全部无向边并按权值升序排序，用并查集（路径压缩 + 按秩合并）逐条选边，不成环即加入，得到 V-1 条边后结束。连通图下两者总权值必然相等，测试以该性质互验；图不连通时 Prim 返回可达部分生成树、Kruskal 返回各连通分量的生成森林（edge_count < V-1 标识）。")
    para(doc, "3.4 最短路径：Floyd（O(V³)）与 Dijkstra（O(V²)）", size=12, bold=True)
    para(doc, "Floyd-Warshall 动态规划求全源最短路径：dist[i][j] 记录最短权值和，next[i][j] 记录路径上 i 的下一个节点用于路径重构；三层循环以 k 为中间点松弛。shortest_floyd_path 沿 next 链重建路径，不可达时返回空路径。Dijkstra 采用教材经典 O(V²) 版本（数组选最小未定节点，无堆），按用户名查得起讫下标后求单源最短路径，沿 prev 回溯重建路径序列；权值均为正（(0,1)），满足使用条件。")
    para(doc, "3.5 工程化措施", size=12, bold=True)
    para(doc, "为保证正确性与可维护性，配套了：① 8 组单元/集成测试（test_util.h 提供轻量断言；包含“算法互相校验”型断言——Prim 与 Kruskal 总权值一致、任意点对 Floyd 与 Dijkstra 距离一致且沿路径权值和正确、BFS 各距离层恰为全用户的一个划分）；② 端到端集成测试直接加载真实数据文件；③ GitHub Actions 三条流水线：严格告警（-Wall -Wextra -Wpedantic -Werror）编译与测试、ASan/UBSan 与 valgrind 内存/未定义行为检测、示例会话自动演示 demo.sh；④ 输入与资源安全：文件格式错误拒绝加载、邻接表用完即释放、退出时整树清理。")

    # ================= 4 运行/测试结果 =================
    heading(doc, "4. 运行/测试结果")
    para(doc, "4.1 单元与集成测试", size=12, bold=True)
    para(doc, "make test 编译并运行 8 个测试可执行文件，共 2513 项断言全部通过（0 failed）：")
    for t in [
        "  test_user 122 项：用户增删查、编号/姓名唯一、年龄与长度边界、满员边界；",
        "  test_storage 649 项：加载需求文档示例（8 用户）与 20 用户数据、矩阵对称性与取值、保存-加载往返一致、错误文件；",
        "  test_graph 48 项：好友增删的对称性、非法输入、连通性判定、邻接表与矩阵一致；",
        "  test_traversal 23 项：DFS(栈/递归)/BFS 遍历全覆盖、K 距离正确、非法起点与越界 K；",
        "  test_mst 20 项：手算 MST 总权值、Prim/Kruskal 互验、树性质（删任意边必不连通）、不连通/空图；",
        "  test_shortest_floyd 14 项：手算最短路与路径重构合法性、不可达/越界；",
        "  test_shortest_dijkstra 11 项：手算最短路、反向路径、起讫相同、不可达/越界；",
        "  test_e2e 1626 项：加载真实数据文件后的跨模块一致性（MST、Floyd==Dijkstra、BFS 距离层划分、好友增删往返）。",
    ]:
        para(doc, t)
    para(doc, "（此处插入图 1：make test 终端运行截图，全部 8 项 PASS）")
    para(doc, "4.2 示例会话（与需求文档“表 1”的 8 用户示例对应）", size=12, bold=True)
    para(doc, "运行 ./build/social_network data/users8.txt 后按菜单操作，示例输出（完整转录见 examples/session8.out.txt）关键结果摘录如下：")
    code(doc, "（菜单：1 显示所有用户 / 2 关系图 / 3-4 好友增删 / 5-6 DFS / 7 BFS / 8 BFS 距离K / 9-10 最小生成树 / 11-12 最短路径 / 13 保存 / 0 退出）")
    for t in [
        "  · BFS 距离 K：与 AlexWhite 距离为 1 的用户 → SophiaLee、DanielGreen、AvaJones；",
        "  · Prim 最小生成树：7 条边，总权值 2.702；Kruskal 最小生成树总权值同样为 2.702（两算法互验一致）；",
        "  · Floyd 与 Dijkstra 均输出 AlexWhite → AvaJones 的最短路径（含路径节点与权值和，两算法结果一致）。",
    ]:
        para(doc, t)
    para(doc, "（此处插入图 2：示例会话终端截图，可对照 examples/session8.out.txt）")
    para(doc, "4.3 GitHub Actions 持续集成", size=12, bold=True)
    for t in [
        "主分支每次推送自动运行三条流水线，最终状态全部绿色：",
        "  · build & test (gcc, warnings-as-errors)：-Wall -Wextra -Wpedantic -Werror 严格编译 + 全量测试；",
        "  · sanitize：ASan/UBSan 重跑全部测试 + valgrind 泄漏检查（退出码检测，0 报错）；",
        "  · demo：examples/demo.sh 自动演示 11 项关键输出 PASS。",
    ]:
        para(doc, t)
    para(doc, "（此处插入图 3：GitHub Actions 三次流水线全绿截图）")

    # ================= 5 项目总结 =================
    heading(doc, "5. 项目总结")
    para(doc, "5.1 开发经过", size=12, bold=True)
    para(doc, "项目按“规格先行、接口契约、功能 Issue、并行 worktree、依赖顺序合入、CI 守门”的流程推进：第一步把老师需求文档精简为设计方案 spec（DESIGN.md：数据结构、算法、存储格式、模块接口），并在 main 上固定全部公共头文件作为并行开发契约；第二步按功能拆分 10 个 Issue（系统输入与文件存储、用户管理、好友关系管理、图遍历、最小生成树、Floyd、Dijkstra、交互界面、样例与集成测试、CI 工作流），每个 Issue 一个独立 worktree 分支并行开发，分支内自带单元测试自证；第三步按依赖顺序合入 main（CI 分支先合入使后续每次合入都触发流水线），补齐集成阶段的跨模块修复（如非连通图 MST 语义、浮点精度断言），最终 main 上 8 组测试全绿、示例会话转录入册、CI 三条流水线全绿。合并历史中每个分支通过“Closes #N”自动关闭对应 Issue，GitHub 记录了完整的一功能一 Issue 的开发轨迹。")
    para(doc, "5.2 人与 AI 的分工", size=12, bold=True)
    para(doc, "本项目的代码整体由 AI 实现（公共接口由作者与 AI 共同在 spec 阶段确定），作者负责全部评审与决策：逐分支 review（REVIEW.md 留有按分支的评审记录与合入 commit），评审点包括接口契约一致性、算法正确性与复杂度、边界处理、资源释放、本分支测试通过与内存检测通过；合入后作者还复核了集成阶段的每处修复。两处典型决策：① 非连通图下 Prim/Kruskal 的容错语义（前者返回可达部分生成树、后者返回生成森林）由作者确认后写入 spec 并测试固化；② 浮点路径断言的精度处理（float 累加误差）在作者确认下从“精确相等”改为阈值比较。")
    para(doc, "5.3 收获与反思", size=12, bold=True)
    for t in [
        "（1）接口契约先行是并行开发的命门：十个分支在 main 上只共享头文件与 Makefile，功能代码互不重叠，全程零合并冲突；若临时需要约定（如测试链接规则），修在 main 上并立刻同步到各 worktree。",
        "（2）算法正确性要“交叉验证”不要“单点断言”：同一问题用两套算法实现互验（Prim 对 Kruskal、Floyd 对 Dijkstra），再用“沿路径权值和 == 最短距离”“BFS 各层恰为全体用户的一个划分”等性质作结构性约束，比死记硬背一组期望值可靠得多；测试中几处手算期望与实现细节不一致，正是被性质约束揪出来的。",
        "（3）AI 是放大器而不是替代品：AI 让“系统化、工程化”的连招（spec、CI、内存检测、样例演示）推进得又快又完整，但前提是作者能解释每一处设计与每一个取舍——本项目中所有关键决策都先经作者确认，任何“想当然”都会很快被测试或 CI 放大为失败。",
        "（4）内存与输入安全的习惯：邻接表动态节点用完即释放、退出时整树清理、valgrind/ASan 常态化进 CI，是 C 语言项目“进得了评审”的基本功。",
    ]:
        para(doc, t)

    # ================= 附：项目代码 =================
    para(doc, "附：项目代码", size=14, bold=True, east="黑体", space_before=12)
    para(doc, "源码清单：include/（7 个公共头文件）、src/（9 个实现文件）、tools/gen_data.c、Makefile、tests/（test_util.h + 8 组测试）、.github/workflows/ci.yml、examples/demo.sh。源码依次附后。（文件按上述清单顺序）")
    para(doc, "（一）公共头文件 include/", size=12, bold=True)
    appendix(doc, [
        "include/user.h", "include/graph.h", "include/traversal.h",
        "include/mst.h", "include/shortest.h", "include/storage.h", "include/ui.h",
    ], "")
    para(doc, "（二）实现源码 src/ 与工具", size=12, bold=True)
    appendix(doc, [
        "src/user.c", "src/graph.c", "src/storage.c", "src/traversal.c",
        "src/mst.c", "src/floyd.c", "src/dijkstra.c", "src/ui.c", "src/main.c",
        "tools/gen_data.c", "Makefile",
    ], "")
    para(doc, "（三）测试与 CI", size=12, bold=True)
    appendix(doc, [
        "tests/test_util.h", "tests/test_user.c", "tests/test_storage.c",
        "tests/test_graph.c", "tests/test_traversal.c", "tests/test_mst.c",
        "tests/test_shortest_floyd.c", "tests/test_shortest_dijkstra.c",
        "tests/test_e2e.c", ".github/workflows/ci.yml", "examples/demo.sh",
    ], "")

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    doc.save(OUT)
    print("已生成:", OUT)


if __name__ == "__main__":
    main()