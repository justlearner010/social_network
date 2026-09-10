#ifndef SN_GRAPH_H
#define SN_GRAPH_H

#include "user.h"

/* 0.0 表示无边；权值 (0,1)，越小亲密度越高 */
#define EDGE_NONE 0.0f
/* 算法中使用的"无穷大" */
#define GRAPH_INF 1e9f

/* 图的邻接矩阵表示 */
typedef struct Graph {
    float adj[MAX_USERS][MAX_USERS];
    int   num_users;
} Graph;

/* 图的邻接表表示（用于图遍历） */
typedef struct Edge {      /* 邻接链表的边节点 */
    int to;                /* 朋友节点下标 */
    float weight;          /* 亲密度权值 */
    struct Edge *next;
} Edge;

typedef struct AdjNode {   /* 顶点节点 */
    User user;             /* 用户信息副本 */
    Edge *head;            /* 邻接链表头 */
} AdjNode;

typedef struct AdjList {
    AdjNode nodes[MAX_USERS];
    int num_users;
} AdjList;

void graph_init(Graph *g, int num_users);
int  graph_valid_index(const Graph *g, int i);

/* 好友关系管理：添加好友（设置亲密度 w，需 0 < w ≤ 1） */
int graph_set_edge(Graph *g, int u, int v, float w);
/* 删除好友 */
int graph_del_edge(Graph *g, int u, int v);
bool graph_has_edge(const Graph *g, int u, int v);
float graph_weight(const Graph *g, int u, int v);
/* 判断图（无向）是否连通：任意两点可达 */
bool graph_is_connected(const Graph *g);

/* 关系图显示：边列表 / 邻接矩阵 */
void graph_print_edges(const Graph *g, const UserDB *db);
void graph_print_matrix(const Graph *g, const UserDB *db);

/* 邻接表构建与释放 */
AdjList adjlist_build(const Graph *g, const UserDB *db);
void    adjlist_free(AdjList *al);

#endif /* SN_GRAPH_H */