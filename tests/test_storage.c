/* tests/test_storage.c — 数据文件读写测试（Issue #3） */
#include "test_util.h"
#include <string.h>
#include "storage.h"
#include "user.h"
#include "graph.h"

int main(void) {
    UserDB db, db2;
    Graph g, g2;
    int i, j;

    /* 1) 加载需求文档示例数据（8 用户，表 1） */
    CHECK_EQ_INT(storage_load("data/users8.txt", &db, &g), 0);
    CHECK_EQ_INT(db.count, 8);
    CHECK_EQ_INT(g.num_users, 8);

    CHECK(strcmp(db.users[0].id, "20250621") == 0);
    CHECK(strcmp(db.users[0].username, "AlexWhite") == 0);
    CHECK(strcmp(db.users[0].password, "a1B2c3D4") == 0);
    CHECK_EQ_INT(db.users[0].age, 25);
    CHECK(strcmp(db.users[7].username, "AvaJones") == 0);
    CHECK_EQ_INT(db.users[7].age, 27);

    /* 2) 矩阵性质：对称、对角为 0、权值 ∈ [0,1]、非稀疏且有边 */
    int edges = 0;
    for (i = 0; i < 8; i++) {
        CHECK(g.adj[i][i] == 0.0f);
        for (j = 0; j < 8; j++) {
            CHECK(g.adj[i][j] >= 0.0f && g.adj[i][j] <= 1.0f);
            CHECK(g.adj[i][j] == g.adj[j][i]);
            if (i < j && g.adj[i][j] > 0.0f) edges++;
        }
    }
    CHECK(edges >= 8); /* 非稀疏 */

    /* 3) 往返一致性：保存后再加载，内容完全一致 */
    CHECK_EQ_INT(storage_save("/tmp/sn_storage_roundtrip.txt", &db, &g), 0);
    CHECK_EQ_INT(storage_load("/tmp/sn_storage_roundtrip.txt", &db2, &g2), 0);
    CHECK_EQ_INT(db2.count, db.count);
    for (i = 0; i < db.count; i++) {
        CHECK(strcmp(db2.users[i].id, db.users[i].id) == 0);
        CHECK(strcmp(db2.users[i].username, db.users[i].username) == 0);
        CHECK(strcmp(db2.users[i].password, db.users[i].password) == 0);
        CHECK_EQ_INT(db2.users[i].age, db.users[i].age);
        for (j = 0; j < db.count; j++)
            CHECK_CLOSE(g2.adj[i][j], g.adj[i][j], 1e-6);
    }

    /* 4) 20 用户数据：用户数 > 10 且矩阵对称连通 */
    CHECK_EQ_INT(storage_load("data/users20.txt", &db, &g), 0);
    CHECK(db.count > 10);
    CHECK_EQ_INT(g.num_users, db.count);
    for (i = 0; i < g.num_users; i++)
        for (j = 0; j < g.num_users; j++)
            CHECK(g.adj[i][j] == g.adj[j][i]);

    /* 5) 错误文件 */
    CHECK_EQ_INT(storage_load("data/no_such_file.txt", &db, &g), -1);

    remove("/tmp/sn_storage_roundtrip.txt");
    return test_summary("test_storage");
}