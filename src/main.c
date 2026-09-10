/* src/main.c — 程序入口
 *
 * 用法: social_network [数据文件]
 *   默认加载 data/users20.txt（20 位用户，满足"用户数 > 10"）
 *   示例: social_network data/users8.txt
 */
#include "graph.h"
#include "storage.h"
#include "ui.h"
#include "user.h"

#include <stdio.h>

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "data/users20.txt";
    UserDB db;
    Graph g;

    if (storage_load(path, &db, &g) != 0) {
        fprintf(stderr, "加载数据失败: %s\n", path);
        return 1;
    }

    printf("已加载 %d 位用户（数据文件 %s）\n", db.count, path);
    return ui_run(&db, &g, path);
}