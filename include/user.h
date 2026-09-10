#ifndef SN_USER_H
#define SN_USER_H

#include <stdbool.h>

/* 社交网络中最大用户数（同需求文档参考实现） */
#define MAX_USERS 100

#define USER_ID_LEN   10
#define USER_NAME_LEN 20
#define USER_PASS_LEN 20

/* 用户结构体：编号 / 姓名 / 密码 / 年龄 */
typedef struct User {
    char id[USER_ID_LEN];
    char username[USER_NAME_LEN];
    char password[USER_PASS_LEN];
    int  age;
} User;

/* 用户库：顺序存储（数组 + 规模） */
typedef struct UserDB {
    User users[MAX_USERS];
    int  count;
} UserDB;

void userdb_init(UserDB *db);

/* 添加用户；成功返回新用户下标，失败（已满/编号或姓名冲突）返回 -1 */
int  userdb_add(UserDB *db, const User *u);
/* 删除下标 index 处的用户；成功返回 0，越界返回 -1 */
int  userdb_remove(UserDB *db, int index);
/* 按编号查找，返回下标；未找到返回 -1 */
int  userdb_find_by_id(const UserDB *db, const char *id);
/* 按姓名查找，返回下标；未找到返回 -1 */
int  userdb_find_by_name(const UserDB *db, const char *name);
const User *userdb_get(const UserDB *db, int index);
void userdb_print(const UserDB *db);

#endif /* SN_USER_H */