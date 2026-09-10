/* src/ui.c — 交互菜单（显示关系图 + 各类算法调用）
 *
 * 菜单项：
 *   1 显示所有用户    2 显示关系图       3 添加好友       4 删除好友
 *   5 DFS 遍历        6 BFS 遍历         7 BFS 距离 K     8 Prim 最小生成树
 *   9 Kruskal 最小生成树  10 Floyd 最短路径  11 Dijkstra 最短路径
 *   12 保存数据       0 退出
 */
#include "ui.h"

#include "mst.h"
#include "shortest.h"
#include "storage.h"
#include "traversal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 读取一行输入（去除换行） */
static void read_line(char *buf, size_t sz) {
    if (fgets(buf, (int)sz, stdin) == NULL)
        buf[0] = '\0';
    buf[strcspn(buf, "\r\n")] = '\0';
}

/* 交互选择用户下标（按姓名）；失败返回 -1 */
static int pick_user(const UserDB *db, const char *prompt) {
    char name[USER_NAME_LEN * 2];
    int idx;
    printf("%s", prompt);
    read_line(name, sizeof name);
    idx = userdb_find_by_name(db, name);
    if (idx < 0)
        printf("未找到用户: %s\n", name);
    return idx;
}

static double read_weight(void) {
    char buf[32];
    printf("输入亲密度（0,1]，越小越亲密；直接回车随机生成: ");
    read_line(buf, sizeof buf);
    if (buf[0] == '\0') /* 随机生成 (0,1) 权值 */
        return (double)(rand() % 999 + 1) / 1000.0;
    return atof(buf);
}

/* ---------- 菜单命令 ---------- */

static void cmd_list_users(UserDB *db) {
    userdb_print(db);
}

static void cmd_show_graph(UserDB *db, Graph *g) {
    graph_print_matrix(g, db);
    graph_print_edges(g, db);
}

static void cmd_add_friend(UserDB *db, Graph *g) {
    int a = pick_user(db, "用户 A 姓名: ");
    int b = pick_user(db, "用户 B 姓名: ");
    if (a < 0 || b < 0)
        return;
    double w = read_weight();
    if (w <= 0.0 || w > 1.0) {
        printf("亲密度需在 (0,1] 内\n");
        return;
    }
    if (graph_set_edge(g, a, b, (float)w) == 0)
        printf("已添加好友 %s <-> %s (%.3f)\n",
               db->users[a].username, db->users[b].username, g->adj[a][b]);
    else
        printf("添加失败\n");
}

static void cmd_del_friend(UserDB *db, Graph *g) {
    int a = pick_user(db, "用户 A 姓名: ");
    int b = pick_user(db, "用户 B 姓名: ");
    if (a < 0 || b < 0)
        return;
    if (graph_del_edge(g, a, b) == 0)
        printf("已删除好友 %s <-> %s\n", db->users[a].username, db->users[b].username);
    else
        printf("删除失败\n");
}

static AdjList build_al(const Graph *g, const UserDB *db) {
    return adjlist_build(g, db);
}

static void cmd_dfs(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    if (s < 0) return;
    AdjList al = build_al(g, db);
    VisitResult r = traversal_dfs(&al, s);
    printf("DFS（显式栈）访问顺序: ");
    visit_print(&r, db);
    adjlist_free(&al);
}

static void cmd_dfs_recursive(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    if (s < 0) return;
    AdjList al = build_al(g, db);
    VisitResult r = traversal_dfs_recursive(&al, s);
    printf("DFS（递归）访问顺序: ");
    visit_print(&r, db);
    adjlist_free(&al);
}

static void cmd_bfs(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    if (s < 0) return;
    AdjList al = build_al(g, db);
    VisitResult r = traversal_bfs(&al, s);
    printf("BFS 访问顺序: ");
    visit_print(&r, db);
    adjlist_free(&al);
}

static void cmd_bfs_k(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    if (s < 0) return;
    char buf[16];
    printf("K 值: ");
    read_line(buf, sizeof buf);
    int k = atoi(buf);
    if (k < 0) {
        printf("K 需非负\n");
        return;
    }
    AdjList al = build_al(g, db);
    DistResult d = traversal_bfs_distance_k(&al, s, k);
    printf("与 %s 距离为 %d 的用户: \n", db->users[s].username, k);
    dist_print(&d, db);
    adjlist_free(&al);
}

static void cmd_mst_prim(UserDB *db, Graph *g) {
    MstResult r = mst_prim(g);
    mst_print(&r, db);
}

static void cmd_mst_kruskal(UserDB *db, Graph *g) {
    MstResult r = mst_kruskal(g);
    mst_print(&r, db);
}

static void cmd_floyd(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    int t = pick_user(db, "终点用户姓名: ");
    if (s < 0 || t < 0) return;
    PathResult p = shortest_floyd_path(g, s, t);
    printf("Floyd 最短路径 %s -> %s: ", db->users[s].username, db->users[t].username);
    path_print(&p, db);
}

static void cmd_dijkstra(UserDB *db, Graph *g) {
    int s = pick_user(db, "起点用户姓名: ");
    int t = pick_user(db, "终点用户姓名: ");
    if (s < 0 || t < 0) return;
    PathResult p = shortest_dijkstra(g, s, t);
    printf("Dijkstra 最短路径 %s -> %s: ", db->users[s].username, db->users[t].username);
    path_print(&p, db);
}

static void cmd_save(UserDB *db, Graph *g, const char *path) {
    if (storage_save(path, db, g) == 0)
        printf("已保存到 %s\n", path);
    else
        printf("保存失败\n");
}

static void print_menu(void) {
    printf("\n======== 小型社交网络系统 ========\n");
    printf(" 1 显示所有用户       2 显示关系图(矩阵)\n");
    printf(" 3 添加好友           4 删除好友\n");
    printf(" 5 DFS 遍历(显式栈)   6 DFS 遍历(递归)\n");
    printf(" 7 BFS 遍历           8 BFS 距离 K\n");
    printf(" 9 Prim 最小生成树   10 Kruskal 最小生成树\n");
    printf("11 Floyd 最短路径    12 Dijkstra 最短路径\n");
    printf("13 保存数据           0 退出\n");
    printf("请选择: ");
}

int ui_run(UserDB *db, Graph *g, const char *data_path) {
    char buf[16];
    for (;;) {
        int choice;
        print_menu();
        read_line(buf, sizeof buf);
        choice = atoi(buf);
        switch (choice) {
            case 0:  return 0;
            case 1:  cmd_list_users(db); break;
            case 2:  cmd_show_graph(db, g); break;
            case 3:  cmd_add_friend(db, g); break;
            case 4:  cmd_del_friend(db, g); break;
            case 5:  cmd_dfs(db, g); break;
            case 6:  cmd_dfs_recursive(db, g); break;
            case 7:  cmd_bfs(db, g); break;
            case 8:  cmd_bfs_k(db, g); break;
            case 9:  cmd_mst_prim(db, g); break;
            case 10: cmd_mst_kruskal(db, g); break;
            case 11: cmd_floyd(db, g); break;
            case 12: cmd_dijkstra(db, g); break;
            case 13: cmd_save(db, g, data_path); break;
            default: printf("无效选项，请重试\n"); break;
        }
    }
}