/* src/traversal.c — 图遍历应用（基于邻接表）
 *
 * DFS：显式栈（数组模拟）版 + 递归版
 * BFS：循环队列版
 * BFS 距离 K：返回与起点"跳数距离"恰为 K 的用户集合
 */
#include "traversal.h"

#include <stdio.h>

static void dfs_rec_impl(const AdjList *al, int u, int *visited, VisitResult *res) {
    visited[u] = 1;
    res->order[res->len++] = u;
    for (Edge *e = al->nodes[u].head; e; e = e->next)
        if (!visited[e->to])
            dfs_rec_impl(al, e->to, visited, res);
}

VisitResult traversal_dfs_recursive(const AdjList *al, int start) {
    VisitResult res = {0};
    int visited[MAX_USERS] = {0};
    if (start >= 0 && start < al->num_users)
        dfs_rec_impl(al, start, visited, &res);
    return res;
}

/* 显式栈 DFS：弹栈时访问并入栈未访问邻居（逆序入栈保证按升序访问） */
VisitResult traversal_dfs(const AdjList *al, int start) {
    VisitResult res = {0};
    int visited[MAX_USERS] = {0};
    int stack[MAX_USERS];
    int top = 0, u;

    if (start < 0 || start >= al->num_users)
        return res;
    stack[top++] = start;
    while (top > 0) {
        u = stack[--top];
        if (visited[u])
            continue;
        visited[u] = 1;
        res.order[res.len++] = u;
        /* 逆序扫描邻居并压栈，使出栈顺序为升序（结果确定可测） */
        int nbr[MAX_USERS], cnt = 0;
        for (Edge *e = al->nodes[u].head; e; e = e->next)
            if (!visited[e->to])
                nbr[cnt++] = e->to;
        for (int i = cnt - 1; i >= 0; i--)
            stack[top++] = nbr[i];
    }
    return res;
}

/* BFS：循环队列 */
VisitResult traversal_bfs(const AdjList *al, int start) {
    VisitResult res = {0};
    int visited[MAX_USERS] = {0};
    int queue[MAX_USERS], head = 0, tail = 0, u;

    if (start < 0 || start >= al->num_users)
        return res;
    visited[start] = 1;
    queue[tail++] = start;
    res.order[res.len++] = start;
    while (head < tail) {
        u = queue[head++];
        for (Edge *e = al->nodes[u].head; e; e = e->next)
            if (!visited[e->to]) {
                visited[e->to] = 1;
                queue[tail++] = e->to;
                res.order[res.len++] = e->to;
            }
    }
    return res;
}

/* BFS 距离 K：dist[v] = 与 start 的跳数距离；返回 dist == k 的用户 */
DistResult traversal_bfs_distance_k(const AdjList *al, int start, int k) {
    DistResult res = {0};
    int dist[MAX_USERS];
    int queue[MAX_USERS], head = 0, tail = 0, u;

    if (start < 0 || start >= al->num_users || k < 0)
        return res;
    for (u = 0; u < al->num_users; u++)
        dist[u] = -1;
    dist[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        u = queue[head++];
        for (Edge *e = al->nodes[u].head; e; e = e->next)
            if (dist[e->to] < 0) {
                dist[e->to] = dist[u] + 1;
                queue[tail++] = e->to;
            }
    }
    for (u = 0; u < al->num_users; u++)
        if (dist[u] == k)
            res.users[res.len++] = u;
    return res;
}

void visit_print(const VisitResult *v, const UserDB *db) {
    int i;
    for (i = 0; i < v->len; i++) {
        const char *name = (db && v->order[i] < db->count) ? db->users[v->order[i]].username : "?";
        if (i > 0) printf(" -> ");
        printf("%s(%d)", name, v->order[i]);
    }
    printf("\n");
}

void dist_print(const DistResult *d, const UserDB *db) {
    int i;
    if (d->len == 0) {
        printf("（无）\n");
        return;
    }
    for (i = 0; i < d->len; i++) {
        const char *name = (db && d->users[i] < db->count) ? db->users[d->users[i]].username : "?";
        printf("  %s(%d)\n", name, d->users[i]);
    }
}