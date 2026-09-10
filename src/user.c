/* src/user.c — 用户管理（记录用户规模与基本信息）
 *
 * 顺序存储结构：数组 + count；删除用尾元素覆盖（O(1)）。
 */
#include "user.h"

#include <stdio.h>
#include <string.h>

void userdb_init(UserDB *db) {
    db->count = 0;
}

int userdb_add(UserDB *db, const User *u) {
    int i;
    if (db->count >= MAX_USERS) {
        fprintf(stderr, "[user] 用户已满（%d/%d）\n", db->count, MAX_USERS);
        return -1;
    }
    /* 字段完整性校验 */
    if (u->id[0] == '\0' || u->username[0] == '\0' || u->password[0] == '\0') {
        fprintf(stderr, "[user] 编号/姓名/密码不能为空\n");
        return -1;
    }
    if ((int)strlen(u->id) >= USER_ID_LEN ||
        (int)strlen(u->username) >= USER_NAME_LEN ||
        (int)strlen(u->password) >= USER_PASS_LEN) {
        fprintf(stderr, "[user] 字段超长（编号<%d 姓名<%d 密码<%d）\n",
                USER_ID_LEN, USER_NAME_LEN, USER_PASS_LEN);
        return -1;
    }
    if (u->age < 0 || u->age > 200) {
        fprintf(stderr, "[user] 年龄非法: %d\n", u->age);
        return -1;
    }
    /* 编号、姓名全局唯一 */
    if (userdb_find_by_id(db, u->id) >= 0) {
        fprintf(stderr, "[user] 编号已存在: %s\n", u->id);
        return -1;
    }
    if (userdb_find_by_name(db, u->username) >= 0) {
        fprintf(stderr, "[user] 姓名已存在: %s\n", u->username);
        return -1;
    }
    db->users[db->count] = *u;
    return db->count++;
}

int userdb_remove(UserDB *db, int index) {
    if (index < 0 || index >= db->count)
        return -1;
    db->users[index] = db->users[db->count - 1]; /* 尾元素覆盖，O(1) */
    db->count--;
    return 0;
}

int userdb_find_by_id(const UserDB *db, const char *id) {
    int i;
    for (i = 0; i < db->count; i++)
        if (strcmp(db->users[i].id, id) == 0)
            return i;
    return -1;
}

int userdb_find_by_name(const UserDB *db, const char *name) {
    int i;
    for (i = 0; i < db->count; i++)
        if (strcmp(db->users[i].username, name) == 0)
            return i;
    return -1;
}

const User *userdb_get(const UserDB *db, int index) {
    if (index < 0 || index >= db->count)
        return NULL;
    return &db->users[index];
}

void userdb_print(const UserDB *db) {
    int i;
    printf("共 %d 位用户\n", db->count);
    printf("%-10s %-18s %-10s %-4s\n", "编号", "姓名", "密码", "年龄");
    for (i = 0; i < db->count; i++) {
        const User *u = &db->users[i];
        printf("%-10s %-18s %-10s %-4d\n", u->id, u->username, u->password, u->age);
    }
}