/* src/floyd.c — 最短路径应用：弗洛伊德（Floyd-Warshall）全源最短路径
 *
 * O(V^3) 动态规划；dist[][] 最短权值和，next[][] 用于路径重构。
 * 要求：输入图权值为正（(0,1)），退化到不可达对返回空路径。
 */
#include "shortest.h"

#include <stdio.h>
#include <string.h>

void shortest_floyd(const Graph *g,
                    float dist[MAX_USERS][MAX_USERS],
                    int next[MAX_USERS][MAX_USERS]) {
    int n = g->num_users;
    int i, j, k;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) {
                dist[i][j] = 0.0f;
                next[i][j] = i;
            } else if (g->adj[i][j] > EDGE_NONE) {
                dist[i][j] = g->adj[i][j];
                next[i][j] = j;
            } else {
                dist[i][j] = GRAPH_INF;
                next[i][j] = -1;
            }
        }
    }
    for (k = 0; k < n; k++)
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                if (dist[i][k] < GRAPH_INF && dist[k][j] < GRAPH_INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
}

PathResult shortest_floyd_path(const Graph *g, int s, int t) {
    PathResult r = {0};
    float dist[MAX_USERS][MAX_USERS];
    int next[MAX_USERS][MAX_USERS];
    int cur, hops = 0;

    if (g->num_users <= 0 || s < 0 || s >= g->num_users ||
        t < 0 || t >= g->num_users)
        return r;

    shortest_floyd(g, dist, next);
    if (next[s][t] < 0)
        return r; /* 不可达 */

    /* 沿 next 链重构路径（最多 n 步，防异常） */
    cur = s;
    while (cur != t && hops++ < g->num_users) {
        if (r.len >= MAX_USERS) break;
        r.nodes[r.len++] = cur;
        cur = next[cur][t];
    }
    if (cur != t)
        return r; /* 异常路径（不应发生） */
    r.nodes[r.len++] = t;
    r.dist = dist[s][t];
    return r;
}

void path_print(const PathResult *p, const UserDB *db) {
    int i;
    if (p->len == 0) {
        printf("（不可达）\n");
        return;
    }
    for (i = 0; i < p->len; i++) {
        const char *name = (db && p->nodes[i] < db->count) ? db->users[p->nodes[i]].username : "?";
        if (i > 0) printf(" -> ");
        printf("%s(%d)", name, p->nodes[i]);
    }
    printf("    路径长度: %.3f\n", p->dist);
}