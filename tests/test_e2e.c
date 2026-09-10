/* tests/test_e2e.c — 端到端集成测试（真实数据文件）
 *
 * 交叉校验各模块的一致性：
 *   - 数据文件为连通非稀疏图
 *   - Prim 与 Kruskal 总权值一致
 *   - Floyd 与 Dijkstra 对任意点对的距离/路径一致，且路径权值和正确
 *   - BFS 距离 K 按跳数对全部用户划分（各 K 层并集 = 全体）
 *   - 好友增删影响连通性并可恢复
 */
#include "test_util.h"
#include "graph.h"
#include "mst.h"
#include "shortest.h"
#include "storage.h"
#include "traversal.h"
#include <math.h>
#include <string.h>

static void test_file(const char *path) {
    UserDB db;
    Graph g;
    int n, s, t;

    CHECK_EQ_INT(storage_load(path, &db, &g), 0);
    n = db.count;
    CHECK_EQ_INT(g.num_users, n);
    CHECK(n > 10); /* 需求：用户数大于 10（users8 为例外示例，此处用 users20 校验） */

    /* 非稀疏连通 */
    int edges = 0;
    for (s = 0; s < n; s++)
        for (t = s + 1; t < n; t++)
            if (g.adj[s][t] > 0.0f) edges++;
    CHECK(edges >= n); /* 非稀疏（至少 n 条边） */
    CHECK(graph_is_connected(&g));

    /* MST 一致性 */
    MstResult p = mst_prim(&g), k = mst_kruskal(&g);
    CHECK_EQ_INT(p.edge_count, n - 1);
    CHECK_EQ_INT(k.edge_count, n - 1);
    CHECK_CLOSE(p.total_weight, k.total_weight, 1e-2f);

    /* Floyd == Dijkstra：全部点对 */
    for (s = 0; s < n; s++) {
        for (t = 0; t < n; t++) {
            PathResult fp = shortest_floyd_path(&g, s, t);
            PathResult dp = shortest_dijkstra(&g, s, t);
            CHECK_EQ_INT(fp.len, dp.len);
            if (fp.len > 0) {
                CHECK_CLOSE(fp.dist, dp.dist, 1e-2f);
                /* 路径相邻点均有边，且权值和 == dist */
                float sum = 0.0f;
                int ok = 1;
                for (int i = 0; i + 1 < fp.len; i++) {
                    if (g.adj[fp.nodes[i]][fp.nodes[i + 1]] <= 0.0f) ok = 0;
                    sum += g.adj[fp.nodes[i]][fp.nodes[i + 1]];
                }
                CHECK(ok);
                CHECK_CLOSE(sum, fp.dist, 1e-2f);
            }
        }
    }

    /* BFS 距离 K：各层是否恰好划分全部用户 */
    AdjList al = adjlist_build(&g, &db);
    {
        int seen[MAX_USERS] = {0}, covered = 0, ok_dist = 1;
        for (int k = 0; k <= n; k++) {
            DistResult d = traversal_bfs_distance_k(&al, 0, k);
            for (int i = 0; i < d.len; i++) {
                if (seen[d.users[i]]) ok_dist = 0; /* 各层不重叠 */
                seen[d.users[i]] = 1;
                covered++;
            }
        }
        CHECK(ok_dist);
        CHECK_EQ_INT(covered, n);
        /* K=1 层用户均为起点的直接好友 */
        DistResult d1 = traversal_bfs_distance_k(&al, 0, 1);
        for (int i = 0; i < d1.len; i++)
            CHECK(graph_has_edge(&g, 0, d1.users[i]));
    }
    adjlist_free(&al);

    /* 好友删除 → 重新添加：对称移除、边数减少、恢复后仍连通 */
    {
        int ei = -1, ej = -1;
        float ew = 0.0f;
        for (s = 0; s < n && ei < 0; s++)
            for (t = s + 1; t < n; t++)
                if (g.adj[s][t] > 0.0f) {
                    ei = s;
                    ej = t;
                    ew = g.adj[s][t];
                }
        CHECK(ei >= 0);
        if (ei >= 0) {
            int e_after;
            graph_del_edge(&g, ei, ej);
            CHECK(!graph_has_edge(&g, ei, ej));
            e_after = 0;
            for (s = 0; s < n; s++)
                for (t = s + 1; t < n; t++)
                    if (g.adj[s][t] > 0.0f) e_after++;
            CHECK_EQ_INT(e_after, edges - 1);
            graph_set_edge(&g, ei, ej, ew);
            CHECK(graph_has_edge(&g, ei, ej));
            CHECK(graph_is_connected(&g));
        }
    }
}

int main(void) {
    test_file("data/users20.txt"); /* 20 用户：满足 >10 */
    return test_summary("test_e2e");
}