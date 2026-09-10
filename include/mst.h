#ifndef SN_MST_H
#define SN_MST_H

#include "graph.h"

/* 最小生成树的一条边 */
typedef struct MstEdge {
    int u, v;         /* 端点 */
    float weight;     /* 权值 */
} MstEdge;

/* 最小生成树结果 */
typedef struct MstResult {
    MstEdge edges[MAX_USERS - 1];
    int  edge_count;      /* 连通时 = num_users - 1 */
    float total_weight;   /* 总权值（权值和最小） */
} MstResult;

/* Prim 算法（O(V^2)），基于邻接矩阵 */
MstResult mst_prim(const Graph *g);
/* Kruskal 算法（O(E log E)），并查集 + 边排序 */
MstResult mst_kruskal(const Graph *g);

void mst_print(const MstResult *mst, const UserDB *db);

#endif /* SN_MST_H */