/* tools/gen_data.c — 随机生成初始数据文件（连通非稀疏图）
 *
 * 用法: ./build/gen_data <用户数> <随机种子> <输出文件>
 *
 * 生成内容（格式见 DESIGN.md §5）：
 *   1. 用户信息：编号(20250621 起)、姓名(FIRST+LAST 池)、密码(8 位)、年龄(18-40)
 *   2. 对称亲密度矩阵：~65% 密度随机铺边，权值 (0.001,0.999)（越小亲密度越高），
 *      并用本地 BFS 保证生成图为连通图（非稀疏）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 100

static const char *const FIRST[] = {
    "Alex", "Emma", "Jack", "Sophia", "Michael", "Olivia", "Daniel", "Ava",
    "Liam", "Noah", "Mia", "Ethan", "Chloe", "Lucas", "Grace", "Leo",
    "Zoe", "Ryan", "Nora", "Ivy"
};
static const int FIRST_N = (int)(sizeof FIRST / sizeof FIRST[0]);

static const char *const LAST[] = {
    "White", "Clark", "Brown", "Lee", "King", "Wu", "Green", "Jones",
    "Smith", "Davis", "Miller", "Wilson", "Moore", "Taylor", "Anderson",
    "Thomas", "Jackson", "Harris", "Martin", "Thompson"
};
static const int LAST_N = (int)(sizeof LAST / sizeof LAST[0]);

/* 权值 ∈ (0.001, 0.999)，保留三位小数 */
static float rand_weight(void) {
    return (float)(rand() % 999 + 1) / 1000.0f;
}

/* 本地 BFS 判连通（独立实现，不依赖主程序模块） */
static int is_connected(const float m[MAX_N][MAX_N], int n) {
    int visited[MAX_N] = {0};
    int queue[MAX_N], head = 0, tail = 0, cnt = 0;
    visited[0] = 1;
    queue[tail++] = 0;
    cnt++;
    while (head < tail) {
        int u = queue[head++];
        int v;
        for (v = 0; v < n; v++)
            if (m[u][v] > 0.0f && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
                cnt++;
            }
    }
    return cnt == n;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "用法: %s <用户数(2..%d)> <随机种子> <输出文件>\n", argv[0], MAX_N);
        return 2;
    }
    int n = atoi(argv[1]);
    unsigned seed = (unsigned)atoi(argv[2]);
    const char *out = argv[3];
    FILE *fp;
    int i, j;

    if (n < 2 || n > MAX_N) {
        fprintf(stderr, "用户数需在 2..%d 之间\n", MAX_N);
        return 2;
    }
    srand(seed);

    /* 用户信息 */
    char id[MAX_N][10];
    char name[MAX_N][20];
    char pass[MAX_N][9];
    int age[MAX_N];
    for (i = 0; i < n; i++) {
        snprintf(id[i], sizeof id[i], "%d", 20250620 + i + 1);
        snprintf(name[i], sizeof name[i], "%s%s", FIRST[i % FIRST_N],
                 LAST[(i * 7) % LAST_N]); /* 7 与 20 互质 → 姓名唯一 */
        for (j = 0; j < 8; j++) {
            if (j % 2 == 1) {
                pass[i][j] = (char)('0' + rand() % 10);
            } else {
                pass[i][j] = (char)((j % 4 == 0 ? 'a' : 'A') + rand() % 26);
            }
        }
        pass[i][8] = '\0';
        age[i] = 18 + rand() % 23; /* 18..40 */
    }

    /* 亲密度矩阵：~65% 密度 + 保证连通 */
    float m[MAX_N][MAX_N];
    memset(m, 0, sizeof m);
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if (rand() % 100 < 65) {
                float w = rand_weight();
                m[i][j] = m[j][i] = w;
            }
    while (!is_connected(m, n)) { /* 补边直至连通（随机函数生成权值） */
        i = rand() % n;
        j = rand() % n;
        if (i != j && m[i][j] == 0.0f) {
            float w = rand_weight();
            m[i][j] = m[j][i] = w;
        }
    }

    /* 写文件 */
    fp = fopen(out, "w");
    if (!fp) {
        fprintf(stderr, "无法打开输出文件: %s\n", out);
        return 1;
    }
    fprintf(fp, "%d\n", n);
    for (i = 0; i < n; i++)
        fprintf(fp, "%s %s %s %d\n", id[i], name[i], pass[i], age[i]);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            fprintf(fp, "%s%.3f", j ? " " : "", m[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);

    printf("已生成 %d 个用户、连通图矩阵 -> %s (种子 %u)\n", n, out, seed);
    return 0;
}