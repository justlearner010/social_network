#ifndef SN_STORAGE_H
#define SN_STORAGE_H

#include "user.h"
#include "graph.h"

/* 从文件加载用户信息与亲密度矩阵（格式见 DESIGN.md 第 5 节）。
 * 成功返回 0，失败返回 -1（并打印错误信息）。 */
int storage_load(const char *path, UserDB *db, Graph *g);

/* 将当前用户信息与矩阵保存到文件。成功返回 0，失败返回 -1。 */
int storage_save(const char *path, const UserDB *db, const Graph *g);

#endif /* SN_STORAGE_H */