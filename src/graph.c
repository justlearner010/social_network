/* src/graph.c — 好友关系管理（邻接矩阵）+ 邻接表构建
 *
 * adj[u][v]：0.0 无边；(0,1) 亲密度，越小越亲密；对称（无向）。
 */
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void graph_init(Graph *g, int num_users) {
    memset(g->adj, 0, sizeof g->adj);
    g->num_users = num_users;
}

int graph_valid_index(const Graph *g, int i) {
    return i >= 0 && i < g->num_users;
}

/* 添加好友：设置亲密度（0 < w ≤ 1），对称写入 */
int graph_set_edge(Graph *g, int u, int v, float w) {
    if (!graph_valid_index(g, u) || !graph_valid_index(g, v)) {
        fprintf(stderr, "[graph] 下标越界: (%d, %d)\n", u, v);
        return -1;
    }
    if (u == v) {
        fprintf(stderr, "[graph] 不能与自己成为好友\n");
        return -1;
    }
    if (w <= 0.0f || w > 1.0f) {
        fprintf(stderr, "[graph] 亲密度需在 (0,1] 内: %f\n", w);
        return -1;
    }
    g->adj[u][v] = w;
    g->adj[v][u] = w;
    return 0;
}

/* 删除好友 */
int graph_del_edge(Graph *g, int u, int v) {
    if (!graph_valid_index(g, u) || !graph_valid_index(g, v))
        return -1;
    g->adj[u][v] = EDGE_NONE;
    g->adj[v][u] = EDGE_NONE;
    return 0;
}

bool graph_has_edge(const Graph *g, int u, int v) {
    if (!graph_valid_index(g, u) || !graph_valid_index(g, v))
        return false;
    return g->adj[u][v] > EDGE_NONE;
}

float graph_weight(const Graph *g, int u, int v) {
    if (!graph_valid_index(g, u) || !graph_valid_index(g, v))
        return EDGE_NONE;
    return g->adj[u][v];
}

/* 连通性：BFS 从 0 号用户出发，统计可达数 */
bool graph_is_connected(const Graph *g) {
    int visited[MAX_USERS] = {0};
    int queue[MAX_USERS], head = 0, tail = 0, cnt = 0, u, v;
    visited[0] = 1;
    queue[tail++] = 0;
    cnt++;
    while (head < tail) {
        u = queue[head++];
        for (v = 0; v < g->num_users; v++) {
            if (g->adj[u][v] > EDGE_NONE && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
                cnt++;
            }
        }
    }
    return cnt == g->num_users;
}

/* 关系图显示：边列表 */
void graph_print_edges(const Graph *g, const UserDB *db) {
    int u, v, m = 0;
    printf("好友关系（亲密度权值）：\n");
    for (u = 0; u < g->num_users; u++)
        for (v = u + 1; v < g->num_users; v++)
            if (g->adj[u][v] > EDGE_NONE) {
                const char *a = (db && u < db->count) ? db->users[u].username : "?";
                const char *b = (db && v < db->count) ? db->users[v].username : "?";
                printf("  %s <-> %s  (%.3f)\n", a, b, g->adj[u][v]);
                m++;
            }
    printf("共 %d 条好友关系\n", m);
}

/* 关系图显示：邻接矩阵 */
void graph_print_matrix(const Graph *g, const UserDB *db) {
    int u, v;
    int w = 9; /* 列宽 */
    printf("邻接矩阵（亲密度，0=无边）：\n");
    printf("%*s", w, "");
    for (v = 0; v < g->num_users; v++) {
        char label[USER_NAME_LEN + 1];
        snprintf(label, sizeof label, "%s", (db && v < db->count) ? db->users[v].username : "?");
        if ((int)strlen(label) > w) label[w] = '\0';
        printf(" %*s", w, label);
    }
    printf("\n");
    for (u = 0; u < g->num_users; u++) {
        char label[USER_NAME_LEN + 1];
        snprintf(label, sizeof label, "%s", (db && u < db->count) ? db->users[u].username : "?");
        if ((int)strlen(label) > w) label[w] = '\0';
        printf("%*s", w, label);
        for (v = 0; v < g->num_users; v++)
            printf(" %9.3f", g->adj[u][v]);
        printf("\n");
    }
}

/* 邻接表构建：头插法；构建顺序 v 降序 → 链表中邻居按 v 升序 */
AdjList adjlist_build(const Graph *g, const UserDB *db) {
    AdjList al;
    int u, v;
    al.num_users = g->num_users;
    for (u = 0; u < g->num_users; u++) {
        al.nodes[u].user = db ? db->users[u] : (User){0};
        al.nodes[u].head = NULL;
        for (v = g->num_users - 1; v >= 0; v--)
            if (g->adj[u][v] > EDGE_NONE) {
                Edge *e = (Edge *)malloc(sizeof *e);
                if (!e) { adjlist_free(&al); al.num_users = 0; return al; }
                e->to = v;
                e->weight = g->adj[u][v];
                e->next = al.nodes[u].head;
                al.nodes[u].head = e;
            }
    }
    return al;
}

void adjlist_free(AdjList *al) {
    int u;
    for (u = 0; u < al->num_users; u++) {
        Edge *e = al->nodes[u].head;
        while (e) {
            Edge *next = e->next;
            free(e);
            e = next;
        }
        al->nodes[u].head = NULL;
    }
}