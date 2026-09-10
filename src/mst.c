/* src/mst.c — 最小生成树应用
 *
 * mst_prim   ：普里姆算法 O(V^2)，基于邻接矩阵（适合稠密图）
 * mst_kruskal：克鲁斯卡尔算法 O(E log E)，边排序 + 并查集
 *
 * 目标：以最小的"亲密度权值和"把所有用户连成一个紧密群体，并输出 MST。
 * 非连通时提前终止（edge_count < num_users-1）。
 */
#include "mst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- 并查集（Kruskal 辅助） ---------------- */
static int uf_find(int *parent, int x) {
    while (parent[x] != x) {
        parent[x] = parent[parent[x]]; /* 路径压缩 */
        x = parent[x];
    }
    return x;
}

static void uf_union(int *parent, int *rank, int a, int b) {
    a = uf_find(parent, a);
    b = uf_find(parent, b);
    if (a == b) return;
    if (rank[a] < rank[b]) { /* 按秩合并 */
        parent[a] = b;
    } else {
        parent[b] = a;
        if (rank[a] == rank[b]) rank[a]++;
    }
}

/* ---------------- Prim ---------------- */
MstResult mst_prim(const Graph *g) {
    MstResult r = {0};
    int n = g->num_users;
    float dist[MAX_USERS];
    int parent[MAX_USERS];
    bool in_tree[MAX_USERS];
    int iter, u, v;

    for (u = 0; u < n; u++) {
        dist[u] = GRAPH_INF;
        parent[u] = -1;
        in_tree[u] = false;
    }
    if (n <= 0) return r;

    dist[0] = 0.0f; /* 任选 0 号用户为起点 */
    for (iter = 0; iter < n; iter++) {
        /* 选取未入树中切分权值最小的顶点 */
        u = -1;
        float best = GRAPH_INF;
        for (v = 0; v < n; v++)
            if (!in_tree[v] && dist[v] < best) {
                best = dist[v];
                u = v;
            }
        if (u < 0) break; /* 图不连通，剩余顶点不可达 */

        in_tree[u] = true;
        if (parent[u] >= 0) { /* 记录 MST 边 */
            r.edges[r.edge_count].u = parent[u];
            r.edges[r.edge_count].v = u;
            r.edges[r.edge_count].weight = dist[u];
            r.edge_count++;
            r.total_weight += dist[u];
        }
        /* 松弛 */
        for (v = 0; v < n; v++)
            if (!in_tree[v] && g->adj[u][v] > EDGE_NONE &&
                g->adj[u][v] < dist[v]) {
                dist[v] = g->adj[u][v];
                parent[v] = u;
            }
    }
    return r;
}

/* ---------------- Kruskal ---------------- */
typedef struct { int u, v; float w; } SortEdge;

static int cmp_edge(const void *a, const void *b) {
    float wa = ((const SortEdge *)a)->w;
    float wb = ((const SortEdge *)b)->w;
    return (wa > wb) - (wa < wb);
}

MstResult mst_kruskal(const Graph *g) {
    MstResult r = {0};
    int n = g->num_users;
    SortEdge es[MAX_USERS * MAX_USERS];
    int parent[MAX_USERS], rank[MAX_USERS];
    int m = 0, u, v, i;

    for (u = 0; u < n; u++) { /* 收集全部无向边 */
        parent[u] = u;
        rank[u] = 0;
        for (v = u + 1; v < n; v++)
            if (g->adj[u][v] > EDGE_NONE) {
                es[m].u = u;
                es[m].v = v;
                es[m].w = g->adj[u][v];
                m++;
            }
    }
    qsort(es, (size_t)m, sizeof es[0], cmp_edge);

    for (i = 0; i < m; i++) {
        u = es[i].u;
        v = es[i].v;
        if (uf_find(parent, u) != uf_find(parent, v)) {
            uf_union(parent, rank, u, v);
            r.edges[r.edge_count].u = u;
            r.edges[r.edge_count].v = v;
            r.edges[r.edge_count].weight = es[i].w;
            r.edge_count++;
            r.total_weight += es[i].w;
            if (r.edge_count == n - 1) break; /* 已构成生成树 */
        }
    }
    return r;
}

void mst_print(const MstResult *mst, const UserDB *db) {
    int i;
    printf("最小生成树（%d 条边，总权值 %.3f）：\n", mst->edge_count, mst->total_weight);
    for (i = 0; i < mst->edge_count; i++) {
        const char *a = db ? db->users[mst->edges[i].u].username : "?";
        const char *b = db ? db->users[mst->edges[i].v].username : "?";
        printf("  %s <-> %s  (%.3f)\n", a, b, mst->edges[i].weight);
    }
}