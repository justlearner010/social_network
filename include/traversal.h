#ifndef SN_TRAVERSAL_H
#define SN_TRAVERSAL_H

#include "graph.h"

/* 遍历结果：节点的访问顺序 */
typedef struct VisitResult {
    int order[MAX_USERS];
    int len;
} VisitResult;

/* BFS 距离 K 结果：与起点距离恰好为 K 的用户集合 */
typedef struct DistResult {
    int users[MAX_USERS];
    int len;
} DistResult;

/* 显式栈实现的 DFS */
VisitResult traversal_dfs(const AdjList *al, int start);
/* 递归实现的 DFS */
VisitResult traversal_dfs_recursive(const AdjList *al, int start);
/* 循环队列实现的 BFS */
VisitResult traversal_bfs(const AdjList *al, int start);
/* BFS 距离 K：返回与 start 距离（跳数）恰为 k 的用户 */
DistResult  traversal_bfs_distance_k(const AdjList *al, int start, int k);

void visit_print(const VisitResult *v, const UserDB *db);
void dist_print(const DistResult *d, const UserDB *db);

#endif /* SN_TRAVERSAL_H */