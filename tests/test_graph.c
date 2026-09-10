/* tests/test_graph.c — 好友关系管理 / 邻接表测试（Issue #4） */
#include "test_util.h"
#include "graph.h"
#include <string.h>

int main(void) {
    Graph g;
    UserDB db;
    AdjList al;
    int u;

    /* 5 个用户的测试图（直接填 UserDB，不依赖 user.c 模块） */
    memset(&db, 0, sizeof db);
    for (u = 0; u < 5; u++) {
        snprintf(db.users[u].id, sizeof db.users[u].id, "U%d", u);
        snprintf(db.users[u].username, sizeof db.users[u].username, "user%d", u);
        snprintf(db.users[u].password, sizeof db.users[u].password, "p%d", u);
        db.users[u].age = 20 + u;
    }
    db.count = 5;
    graph_init(&g, 5);
    CHECK_EQ_INT(g.num_users, 5);

    /* 添加好友：对称性 */
    CHECK_EQ_INT(graph_set_edge(&g, 0, 1, 0.3f), 0);
    CHECK_EQ_INT(graph_set_edge(&g, 0, 2, 0.5f), 0);
    CHECK_EQ_INT(graph_set_edge(&g, 1, 2, 0.1f), 0);
    CHECK_EQ_INT(graph_set_edge(&g, 1, 3, 0.7f), 0);
    CHECK_EQ_INT(graph_set_edge(&g, 2, 4, 0.4f), 0);
    CHECK(graph_has_edge(&g, 0, 1) && graph_has_edge(&g, 1, 0));
    CHECK(graph_weight(&g, 1, 2) == 0.1f);
    CHECK(graph_weight(&g, 2, 1) == 0.1f);

    /* 非法输入 */
    CHECK_EQ_INT(graph_set_edge(&g, 0, 0, 0.5f), -1);  /* 自环 */
    CHECK_EQ_INT(graph_set_edge(&g, 0, 1, 0.0f), -1);  /* 权值非法 */
    CHECK_EQ_INT(graph_set_edge(&g, 0, 1, 1.5f), -1);
    CHECK_EQ_INT(graph_set_edge(&g, -1, 1, 0.5f), -1);
    CHECK_EQ_INT(graph_set_edge(&g, 0, 99, 0.5f), -1);

    /* 连通性：当前全连通 */
    CHECK(graph_is_connected(&g));
    /* 删除唯一连边 1-3 → 3 孤立 → 不连通 */
    CHECK_EQ_INT(graph_del_edge(&g, 1, 3), 0);
    CHECK(!graph_has_edge(&g, 1, 3));
    CHECK(!graph_is_connected(&g));
    CHECK_EQ_INT(graph_del_edge(&g, 1, 3), 0); /* 幂等 */
    /* 恢复 */
    graph_set_edge(&g, 1, 3, 0.7f);
    CHECK(graph_is_connected(&g));

    /* 邻接表构建：边数、邻居升序、权值一致 */
    al = adjlist_build(&g, &db);
    CHECK_EQ_INT(al.num_users, 5);
    int total = 0;
    for (u = 0; u < al.num_users; u++) {
        int prev = -1;
        CHECK(strcmp(al.nodes[u].user.username, db.users[u].username) == 0);
        for (Edge *e = al.nodes[u].head; e; e = e->next) {
            total++;
            CHECK(e->to > prev); /* 升序 */
            prev = e->to;
            CHECK_CLOSE(e->weight, graph_weight(&g, u, e->to), 1e-6);
        }
    }
    CHECK_EQ_INT(total, 10); /* 5 条无向边 × 2 */
    adjlist_free(&al);
    CHECK(al.nodes[0].head == NULL); /* 释放后头指针清空 */

    return test_summary("test_graph");
}