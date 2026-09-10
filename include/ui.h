#ifndef SN_UI_H
#define SN_UI_H

#include "user.h"
#include "graph.h"

/* 进入交互菜单。data_path 为数据文件路径（用于"保存数据"）。
 * 返回 0 表示正常退出。 */
int ui_run(UserDB *db, Graph *g, const char *data_path);

#endif /* SN_UI_H */