#ifndef SN_SHORTEST_H
#define SN_SHORTEST_H

#include "graph.h"

/* 最短路径结果 */
typedef struct PathResult {
    int nodes[MAX_USERS];  /* 含首尾的路径节点序列 */
    int len;               /* 节点个数；0 表示不可达 */
    float dist;            /* 路径权值之和 */
} PathResult;

/* Floyd-Warshall 全源最短路径：
 *   计算 dist[i][j]（最短权值和）与 next[i][j]（路径重构矩阵，
 *   -1 表示不可达，next[i][i] = i）
 */
void shortest_floyd(const Graph *g,
                    float dist[MAX_USERS][MAX_USERS],
                    int next[MAX_USERS][MAX_USERS]);
/* 基于 next 矩阵查询 s -> t 的最短路径 */
PathResult shortest_floyd_path(const Graph *g, int s, int t);

/* Dijkstra 单源最短路径（O(V^2)，权值为正） */
PathResult shortest_dijkstra(const Graph *g, int s, int t);

void path_print(const PathResult *p, const UserDB *db);

#endif /* SN_SHORTEST_H */