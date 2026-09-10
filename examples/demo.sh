#!/usr/bin/env bash
# examples/demo.sh — 自动演示：驱动社交网络程序并校验关键输出
#
# 用法: bash examples/demo.sh  （需要先 make app）
# 说明：会话输入为 8 用户（需求文档表1）示例；逐项校验输出关键标记。
set -u
cd "$(dirname "$0")/.."
APP=build/social_network

if [ ! -x "$APP" ]; then
  echo "[demo] 未找到 $APP，请先执行 make app（所有模块合入后自动可用）"
  exit 0
fi

fail=0
run() { printf '%b' "$1" | "$APP" data/users8.txt 2>&1; }

check() {
  local desc="$1" input="$2" pattern="$3" out
  out=$(run "$input")
  if printf '%s' "$out" | grep -q "$pattern"; then
    echo "PASS: $desc"
  else
    echo "FAIL: $desc  (未匹配: $pattern)"
    fail=1
  fi
}

check "显示所有用户"        '1\n0\n'                              "共 8 位用户"
check "显示关系图(矩阵)"    '2\n0\n'                              "邻接矩阵"
check "DFS 遍历(显式栈)"    '5\nAlexWhite\n0\n'                   "DFS（显式栈）访问顺序"
check "DFS 遍历(递归)"      '6\nAlexWhite\n0\n'                   "DFS（递归）访问顺序"
check "BFS 遍历"            '7\nAlexWhite\n0\n'                   "BFS 访问顺序"
check "BFS 距离 K(K=1)"     '8\nAlexWhite\n1\n0\n'                "距离为 1 的用户"
check "Prim 最小生成树"     '9\n0\n'                              "最小生成树"
check "Kruskal 最小生成树"  '10\n0\n'                             "最小生成树"
check "Floyd 最短路径"      '11\nAlexWhite\nAvaJones\n0\n'        "路径长度:"
check "Dijkstra 最短路径"   '12\nAlexWhite\nAvaJones\n0\n'        "路径长度:"
check "保存数据"            '13\n0\n'                             "已保存到"

echo
if [ $fail -eq 0 ]; then
  echo "[demo] 全部样例通过 ✔"
else
  echo "[demo] 存在失败样例 ✘"
fi
exit $fail