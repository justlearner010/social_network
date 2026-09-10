/* src/storage.c — 数据文件读写（用户信息 + 亲密度矩阵）
 *
 * 文件格式（DESIGN.md §5）：
 *   第 1 行          : 用户数 N
 *   第 2..N+1 行     : <编号> <姓名> <密码> <年龄>
 *   第 N+2..2N+1 行  : N 行 × N 列浮点数（对称亲密度矩阵，0=无边，(0,1)=亲密度）
 */
#include "storage.h"

#include <stdio.h>
#include <string.h>

static int is_blank(const char *s) {
    while (*s) {
        if (*s != ' ' && *s != '\t' && *s != '\r' && *s != '\n')
            return 0;
        s++;
    }
    return 1;
}

/* 读取一行非空行；返回 NULL 表示文件结束 */
static char *next_line(char *buf, size_t sz, FILE *fp) {
    do {
        if (fgets(buf, (int)sz, fp) == NULL)
            return NULL;
    } while (is_blank(buf));
    buf[strcspn(buf, "\r\n")] = '\0';
    return buf;
}

int storage_load(const char *path, UserDB *db, Graph *g) {
    FILE *fp = fopen(path, "r");
    char buf[256];
    int n, i, j;

    if (!fp) {
        fprintf(stderr, "[storage] 无法打开文件: %s\n", path);
        return -1;
    }

    if (!next_line(buf, sizeof buf, fp) || sscanf(buf, "%d", &n) != 1) {
        fprintf(stderr, "[storage] 文件格式错误（缺少用户数）: %s\n", path);
        fclose(fp);
        return -1;
    }
    if (n < 1 || n > MAX_USERS) {
        fprintf(stderr, "[storage] 用户数 %d 超出范围 (1..%d)\n", n, MAX_USERS);
        fclose(fp);
        return -1;
    }

    userdb_init(db);
    graph_init(g, n);

    /* 用户信息 */
    for (i = 0; i < n; i++) {
        int age;
        char id[USER_ID_LEN + 1], uname[USER_NAME_LEN + 1], pwd[USER_PASS_LEN + 1];
        if (!next_line(buf, sizeof buf, fp) ||
            sscanf(buf, "%9s %19s %19s %d", id, uname, pwd, &age) != 4) {
            fprintf(stderr, "[storage] 第 %d 行用户信息格式错误\n", i + 2);
            fclose(fp);
            return -1;
        }
        if (age < 0 || age > 200) {
            fprintf(stderr, "[storage] 用户 %s 年龄非法: %d\n", uname, age);
            fclose(fp);
            return -1;
        }
        User u;
        snprintf(u.id, sizeof u.id, "%s", id);
        snprintf(u.username, sizeof u.username, "%s", uname);
        snprintf(u.password, sizeof u.password, "%s", pwd);
        u.age = age;
        db->users[i] = u;
    }
    db->count = n;

    /* 亲密度矩阵（对称，读入时强制镜像） */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            float w;
            if (fscanf(fp, " %f", &w) != 1) {
                fprintf(stderr, "[storage] 亲密度矩阵不完整（第 %d 行附近）\n", i);
                fclose(fp);
                return -1;
            }
            if (w < 0.0f) w = 0.0f;
            if (w > 1.0f) w = 1.0f;
            g->adj[i][j] = w;
            g->adj[j][i] = w;
        }
    }
    fclose(fp);
    return 0;
}

int storage_save(const char *path, const UserDB *db, const Graph *g) {
    FILE *fp = fopen(path, "w");
    int i, j;
    if (!fp) {
        fprintf(stderr, "[storage] 无法打开文件: %s\n", path);
        return -1;
    }
    fprintf(fp, "%d\n", db->count);
    for (i = 0; i < db->count; i++) {
        const User *u = &db->users[i];
        fprintf(fp, "%s %s %s %d\n", u->id, u->username, u->password, u->age);
    }
    for (i = 0; i < g->num_users; i++) {
        for (j = 0; j < g->num_users; j++)
            fprintf(fp, "%s%.3f", j ? " " : "", g->adj[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}