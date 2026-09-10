/* tests/test_traversal.c — 图遍历测试（Issue #1）
 * 测试图（6 节点无向）：
 *   0-1, 0-2, 1-3, 1-4, 2-5
 *   距离（跳数）：start=0 -> 1:{1,2}, 2:{3,4,5}, K>=3: 空
 *
 * 直接手工构建邻接表（不依赖 graph.c，保持分支独立）。
 */
#include "test_util.h"
#include "graph.h"
#include "traversal.h"
#include <stdlib.h>
#include <string.h>

static int add_edge(AdjList *al, int u, int v, float w) {
    Edge *e = (Edge *)malloc(sizeof *e);
    if (!e) return -1;
    e->to = v;
    e->weight = w;
    e->next = NULL;
    Edge **p = &al->nodes[u].head; /* 尾插，保持邻居升序 */
    while (*p) p = &(*p)->next;
    *p = e;
    return 0;
}

static AdjList make_test_graph(void) {
    AdjList al;
    memset(&al, 0, sizeof al);
    al.num_users = 6;
    add_edge(&al, 0, 1, 0.5f);
    add_edge(&al, 1, 0, 0.5f);
    add_edge(&al, 0, 2, 0.3f);
    add_edge(&al, 2, 0, 0.3f);
    add_edge(&al, 1, 3, 0.7f);
    add_edge(&al, 3, 1, 0.7f);
    add_edge(&al, 1, 4, 0.2f);
    add_edge(&al, 4, 1, 0.2f);
    add_edge(&al, 2, 5, 0.9f);
    add_edge(&al, 5, 2, 0.9f);
    return al;
}

static void free_al(AdjList *al) {
    for (int i = 0; i < al->num_users; i++) {
        Edge *e = al->nodes[i].head;
        while (e) {
            Edge *n = e->next;
            free(e);
            e = n;
        }
    }
}

/* 结果是否恰好包含 0..5 各一次 */
static int covers_all(const VisitResult *r) {
    int seen[6] = {0};
    if (r->len != 6) return 0;
    for (int i = 0; i < r->len; i++) {
        if (r->order[i] < 0 || r->order[i] >= 6 || seen[r->order[i]]) return 0;
        seen[r->order[i]] = 1;
    }
    return 1;
}

int main(void) {
    AdjList al = make_test_graph();
    VisitResult r;
    DistResult d;

    /* 递归 DFS */
    r = traversal_dfs_recursive(&al, 0);
    CHECK(covers_all(&r));
    CHECK_EQ_INT(r.order[0], 0);

    /* 显式栈 DFS：覆盖全部，且从 0 出发 */
    r = traversal_dfs(&al, 0);
    CHECK(covers_all(&r));
    CHECK_EQ_INT(r.order[0], 0);

    /* BFS：覆盖全部 */
    r = traversal_bfs(&al, 0);
    CHECK(covers_all(&r));
    CHECK_EQ_INT(r.order[0], 0);
    /* 0 的直接邻居按升序紧接其后 */
    CHECK_EQ_INT(r.order[1], 1);
    CHECK_EQ_INT(r.order[2], 2);

    /* 非法起点 */
    r = traversal_dfs(&al, -1);
    CHECK_EQ_INT(r.len, 0);
    r = traversal_bfs(&al, 99);
    CHECK_EQ_INT(r.len, 0);

    /* BFS 距离 K */
    d = traversal_bfs_distance_k(&al, 0, 0);
    CHECK_EQ_INT(d.len, 1);
    CHECK_EQ_INT(d.users[0], 0);

    d = traversal_bfs_distance_k(&al, 0, 1);
    CHECK_EQ_INT(d.len, 2);
    CHECK(d.users[0] == 1 && d.users[1] == 2);

    d = traversal_bfs_distance_k(&al, 0, 2);
    CHECK_EQ_INT(d.len, 3);
    CHECK(d.users[0] == 3 && d.users[1] == 4 && d.users[2] == 5);

    d = traversal_bfs_distance_k(&al, 0, 3);
    CHECK_EQ_INT(d.len, 0);
    d = traversal_bfs_distance_k(&al, 0, -1);
    CHECK_EQ_INT(d.len, 0);

    /* 从 3 出发：3->1->4 为 2 跳；3->1->0->2 为 3 跳；5 为 4 跳 */
    d = traversal_bfs_distance_k(&al, 3, 1);
    CHECK_EQ_INT(d.len, 1);
    CHECK_EQ_INT(d.users[0], 1);
    d = traversal_bfs_distance_k(&al, 3, 2);
    CHECK_EQ_INT(d.len, 2);
    d = traversal_bfs_distance_k(&al, 3, 3);
    CHECK_EQ_INT(d.len, 1);
    CHECK_EQ_INT(d.users[0], 2);

    free_al(&al);
    return test_summary("test_traversal");
}