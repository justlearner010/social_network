/* tests/test_shortest_floyd.c — Floyd 全源最短路径测试（Issue #8）
 *
 * 测试图（5 节点）：
 *   0-1:0.5  0-2:1.0  1-2:0.3  1-3:1.5  2-3:0.4  3-4:0.2
 *   dist[0][3] = 0.5+0.3+0.4 = 1.2（0->1->2->3）
 *   dist[0][4] = 1.2+0.2 = 1.4（0->1->2->3->4）
 */
#include "test_util.h"
#include "shortest.h"
#include <math.h>
#include <string.h>

static void init_g(Graph *g, int n) {
    memset(g->adj, 0, sizeof g->adj);
    g->num_users = n;
}

static void add_edge(Graph *g, int u, int v, float w) {
    g->adj[u][v] = w;
    g->adj[v][u] = w;
}

/* 校验路径合法：相邻节点有边且权值和 == 期望 */
static int check_path(const Graph *g, const PathResult *p, const int *expect, int expect_len,
                      float expect_dist) {
    if (p->len != expect_len) return 0;
    if (p->len > 0 && p->nodes[p->len - 1] != expect[expect_len - 1]) return 0;
    for (int i = 0; i + 1 < p->len; i++)
        if (g->adj[p->nodes[i]][p->nodes[i + 1]] <= 0.0f) return 0;
    float sum = 0.0f;
    for (int i = 0; i + 1 < p->len; i++)
        sum += g->adj[p->nodes[i]][p->nodes[i + 1]];
    return fabs(sum - expect_dist) < 1e-4f;
}

int main(void) {
    Graph g;
    const int exp04[] = {0, 1, 2, 3, 4};
    PathResult pr;
    float dist[MAX_USERS][MAX_USERS];
    int next[MAX_USERS][MAX_USERS];

    init_g(&g, 5);
    add_edge(&g, 0, 1, 0.5f);
    add_edge(&g, 0, 2, 1.0f);
    add_edge(&g, 1, 2, 0.3f);
    add_edge(&g, 1, 3, 1.5f);
    add_edge(&g, 2, 3, 0.4f);
    add_edge(&g, 3, 4, 0.2f);

    shortest_floyd(&g, dist, next);
    CHECK_CLOSE(dist[0][3], 1.2f, 1e-4);
    CHECK_CLOSE(dist[0][4], 1.4f, 1e-4);
    CHECK_CLOSE(dist[0][0], 0.0f, 1e-6);
    CHECK_CLOSE(dist[2][3], 0.4f, 1e-4); /* 直达 */
    CHECK(next[0][3] == 1);

    /* 路径重构 */
    pr = shortest_floyd_path(&g, 0, 4);
    CHECK(check_path(&g, &pr, exp04, 5, 1.4f));

    pr = shortest_floyd_path(&g, 4, 0); /* 双向（无向图） */
    CHECK_EQ_INT(pr.len, 5);
    CHECK_CLOSE(pr.dist, 1.4f, 1e-4);

    pr = shortest_floyd_path(&g, 0, 0);
    CHECK_EQ_INT(pr.len, 1);
    CHECK_CLOSE(pr.dist, 0.0f, 1e-6);

    /* 不连通：3-4 删掉后 4 不可达 */
    init_g(&g, 5);
    add_edge(&g, 0, 1, 0.5f);
    add_edge(&g, 1, 2, 0.3f);
    add_edge(&g, 2, 3, 0.4f);
    shortest_floyd(&g, dist, next);
    CHECK(dist[0][4] >= GRAPH_INF);
    pr = shortest_floyd_path(&g, 0, 4);
    CHECK_EQ_INT(pr.len, 0);

    /* 越界 */
    pr = shortest_floyd_path(&g, -1, 1);
    CHECK_EQ_INT(pr.len, 0);
    pr = shortest_floyd_path(&g, 0, 99);
    CHECK_EQ_INT(pr.len, 0);

    return test_summary("test_shortest_floyd");
}