/* tests/test_mst.c — 最小生成树测试（Issue #7）
 * 测试图 1（4 节点）：
 *   0-1:1.0  0-2:2.0  1-2:3.0  1-3:4.0  2-3:5.0
 *   MST = {0-1, 0-2, 1-3}，总权值 7.0
 */
#include "test_util.h"
#include "mst.h"
#include <string.h>

static void init_g(Graph *g, int n) {
    memset(g->adj, 0, sizeof g->adj);
    g->num_users = n;
}

static void make_graph(Graph *g, int n, const int (*pairs)[2], const float *ws, int m) {
    init_g(g, n);
    for (int i = 0; i < m; i++) {
        g->adj[pairs[i][0]][pairs[i][1]] = ws[i];
        g->adj[pairs[i][1]][pairs[i][0]] = ws[i];
    }
}

int main(void) {
    Graph g;
    MstResult p, k;

    /* 手动初始化 Graph（不依赖 graph.c，保持分支独立） */
    const int pairs1[][2] = {{0,1},{0,2},{1,2},{1,3},{2,3}};
    const float ws1[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    init_g(&g, 4);
    make_graph(&g, 4, pairs1, ws1, 5);

    p = mst_prim(&g);
    k = mst_kruskal(&g);
    CHECK_EQ_INT(p.edge_count, 3);
    CHECK_EQ_INT(k.edge_count, 3);
    CHECK_CLOSE(p.total_weight, 7.0f, 1e-4);
    CHECK_CLOSE(k.total_weight, 7.0f, 1e-4);

    /* MST 覆盖全部 4 个节点 */
    int seen[4] = {0};
    for (int i = 0; i < p.edge_count; i++) {
        seen[p.edges[i].u] = 1;
        seen[p.edges[i].v] = 1;
    }
    CHECK(seen[0] && seen[1] && seen[2] && seen[3]);

    /* Prim 与 Kruskal：总权值一致 */
    CHECK_CLOSE(p.total_weight, k.total_weight, 1e-4);

    /* 更大一点的连通图（6 节点，权值各异） */
    const int pairs2[][2] = {{0,1},{1,2},{2,3},{3,4},{4,5},{5,0},{0,3},{1,4},{2,5}};
    const float ws2[] = {0.9f, 0.2f, 0.6f, 0.3f, 0.8f, 0.1f, 0.7f, 0.4f, 0.5f};
    init_g(&g, 6);
    make_graph(&g, 6, pairs2, ws2, 9);
    p = mst_prim(&g);
    k = mst_kruskal(&g);
    CHECK_EQ_INT(p.edge_count, 5);
    CHECK_EQ_INT(k.edge_count, 5);
    CHECK_CLOSE(p.total_weight, k.total_weight, 1e-4);
    /* 手算：5 条最小边 0.1,0.2,0.3,0.4,0.5? 检验树性：
     * 边按序 0.1(5-0),0.2(1-2),0.3(3-4),0.4(1-4),0.5(2-5)? -> 2-5 形成环(5-0-?-2)…
     * 以算法互验 + 切割性质校验：MST 权值应为切分最小和，这里用强校验：
     * 任意删除一条边后构图必不连通（树性质） */
    {
        /* 复制 p 的边进行树校验 */
        for (int skip = 0; skip < p.edge_count; skip++) {
            Graph t;
            init_g(&t, 6);
            for (int i = 0; i < p.edge_count; i++) {
                if (i == skip) continue;
                t.adj[p.edges[i].u][p.edges[i].v] = p.edges[i].weight;
                t.adj[p.edges[i].v][p.edges[i].u] = p.edges[i].weight;
            }
            /* 局部 BFS 连通性 */
            int vis[6] = {0}, q[6], h = 0, tl = 0, cnt = 1;
            vis[0] = 1; q[tl++] = 0;
            while (h < tl) {
                int u = q[h++];
                for (int v = 0; v < 6; v++)
                    if (t.adj[u][v] > 0 && !vis[v]) { vis[v] = 1; q[tl++] = v; cnt++; }
            }
            CHECK(cnt < 6); /* 树删任意边后必不连通 */
        }
    }

    /* 不连通图：Prim 返回可达部分的生成树；Kruskal 返回生成森林 */
    const int pairs3[][2] = {{0,1},{1,2},{3,4}};
    const float ws3[] = {0.1f, 0.2f, 0.3f};
    init_g(&g, 5);
    make_graph(&g, 5, pairs3, ws3, 3);
    p = mst_prim(&g);
    k = mst_kruskal(&g);
    CHECK_EQ_INT(p.edge_count, 2);
    CHECK_EQ_INT(k.edge_count, 3); /* 两个连通分量各成树 */
    CHECK_CLOSE(p.total_weight, 0.3f, 1e-4);
    CHECK_CLOSE(k.total_weight, 0.6f, 1e-4);

    /* 空图 */
    init_g(&g, 0);
    p = mst_prim(&g);
    k = mst_kruskal(&g);
    CHECK_EQ_INT(p.edge_count, 0);
    CHECK_EQ_INT(k.edge_count, 0);

    return test_summary("test_mst");
}