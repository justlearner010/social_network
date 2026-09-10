/* tests/test_user.c — 用户管理测试（Issue #2） */
#include "test_util.h"
#include "user.h"
#include <string.h>

int main(void) {
    UserDB db;
    User u;
    int idx;

    userdb_init(&db);
    CHECK_EQ_INT(db.count, 0);

    /* 正常添加 */
    memset(&u, 0, sizeof u);
    snprintf(u.id, sizeof u.id, "%s", "20250621");
    snprintf(u.username, sizeof u.username, "%s", "AlexWhite");
    snprintf(u.password, sizeof u.password, "%s", "a1B2c3D4");
    u.age = 25;
    idx = userdb_add(&db, &u);
    CHECK_EQ_INT(idx, 0);
    CHECK_EQ_INT(db.count, 1);

    /* 编号冲突 */
    memset(&u, 0, sizeof u);
    snprintf(u.id, sizeof u.id, "%s", "20250621");
    snprintf(u.username, sizeof u.username, "%s", "EmmaClark");
    snprintf(u.password, sizeof u.password, "%s", "x9Y8z7W6");
    u.age = 30;
    CHECK_EQ_INT(userdb_add(&db, &u), -1);

    /* 姓名冲突 */
    snprintf(u.id, sizeof u.id, "%s", "20250622");
    snprintf(u.username, sizeof u.username, "%s", "AlexWhite");
    CHECK_EQ_INT(userdb_add(&db, &u), -1);

    /* 空字段 */
    snprintf(u.id, sizeof u.id, "%s", "20250622");
    snprintf(u.username, sizeof u.username, "%s", "EmmaClark");
    u.username[0] = '\0';
    CHECK_EQ_INT(userdb_add(&db, &u), -1);
    u.username[0] = 'E';

    /* 年龄边界 */
    snprintf(u.id, sizeof u.id, "%s", "20250622");
    snprintf(u.username, sizeof u.username, "%s", "EmmaClark");
    snprintf(u.password, sizeof u.password, "%s", "x9Y8z7W6");
    u.age = -1;
    CHECK_EQ_INT(userdb_add(&db, &u), -1);
    u.age = 201;
    CHECK_EQ_INT(userdb_add(&db, &u), -1);
    u.age = 30;
    CHECK_EQ_INT(userdb_add(&db, &u), 1);

    /* 查找 */
    CHECK_EQ_INT(userdb_find_by_id(&db, "20250621"), 0);
    CHECK_EQ_INT(userdb_find_by_id(&db, "20250622"), 1);
    CHECK_EQ_INT(userdb_find_by_id(&db, "99999999"), -1);
    CHECK_EQ_INT(userdb_find_by_name(&db, "AlexWhite"), 0);
    CHECK_EQ_INT(userdb_find_by_name(&db, "nobody"), -1);
    CHECK(userdb_get(&db, 0) != NULL);
    CHECK(userdb_get(&db, 99) == NULL);
    CHECK(strcmp(userdb_get(&db, 0)->username, "AlexWhite") == 0);

    /* 删除：尾元素覆盖 */
    CHECK_EQ_INT(userdb_remove(&db, 0), 0);
    CHECK_EQ_INT(db.count, 1);
    CHECK_EQ_INT(userdb_find_by_name(&db, "EmmaClark"), 0);
    CHECK_EQ_INT(userdb_remove(&db, 5), -1); /* 越界 */
    CHECK_EQ_INT(userdb_remove(&db, -1), -1);

    /* 满员边界：填充至 MAX_USERS */
    while (db.count < MAX_USERS) {
        User x;
        char nb[16];
        int n;
        memset(&x, 0, sizeof x);
        n = snprintf(nb, sizeof nb, "%d", 10000000 + db.count);
        memcpy(x.id, nb, (size_t)n); /* 编号至多 8 字节，字段 10 字节足够 */
        x.id[n] = '\0';
        snprintf(x.username, sizeof x.username, "User%d", db.count);
        snprintf(x.password, sizeof x.password, "p%d", db.count);
        x.age = 20;
        CHECK(userdb_add(&db, &x) >= 0);
    }
    CHECK_EQ_INT(userdb_add(&db, &u), -1); /* 已满 */

    return test_summary("test_user");
}