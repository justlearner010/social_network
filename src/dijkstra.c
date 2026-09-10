/* src/dijkstra.c — 最短路径应用：迪杰斯特拉（Dijkstra）单源最短路径
 *
 * 教材经典 O(V^2) 实现（数组选最小未定点，无堆），适配课程深度。
 * 权值均为正（(0,1)），满足 Dijkstra 使用条件；不可达对返回空路径。
 */
#include "shortest.h"

#include <string.h>

PathResult shortest_dijkstra(const Graph *g, int s, int t) {
    PathResult r = {0};
    int n = g->num_users;
    float dist[MAX_USERS];
    int prev[MAX_USERS];
    bool done[MAX_USERS];
    int i, iter, u, v;

    if (n <= 0 || s < 0 || s >= n || t < 0 || t >= n)
        return r;

    for (i = 0; i < n; i++) {
        dist[i] = GRAPH_INF;
        prev[i] = -1;
        done[i] = false;
    }
    dist[s] = 0.0f;

    for (iter = 0; iter < n; iter++) {
        /* 选取未定节点中距离最小者 */
        u = -1;
        float best = GRAPH_INF;
        for (v = 0; v < n; v++)
            if (!done[v] && dist[v] < best) {
                best = dist[v];
                u = v;
            }
        if (u < 0 || u == t)
            break; /* 剩余不可达 或 已确定目标 */
        done[u] = true;

        /* 松弛 */
        for (v = 0; v < n; v++)
            if (!done[v] && g->adj[u][v] > EDGE_NONE &&
                g->adj[u][v] < GRAPH_INF &&
                dist[u] + g->adj[u][v] < dist[v]) {
                dist[v] = dist[u] + g->adj[u][v];
                prev[v] = u;
            }
    }

    if (dist[t] >= GRAPH_INF)
        return r; /* 不可达 */

    /* 沿 prev 回溯并反转 */
    int path[MAX_USERS];
    int cnt = 0;
    int cur = t;
    while (cur != -1 && cnt < MAX_USERS) {
        path[cnt++] = cur;
        cur = prev[cur];
    }
    for (i = 0; i < cnt; i++)
        r.nodes[i] = path[cnt - 1 - i];
    r.len = cnt;
    r.dist = dist[t];
    return r;
}