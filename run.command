#!/bin/bash
# ============================================================
#  小型社交网络系统 —— 一键运行脚本（macOS 双击 / 终端执行）
#
#  用法：
#    macOS：双击 run.command（若提示"无法打开"，请右键 → 打开）
#    终端：./run.sh
#
#  功能：自动编译，然后可选择自动演示 / 交互式体验 / 运行全部测试。
# ============================================================

cd "$(dirname "$0")" || exit 1
REPO="$(pwd)"

GRN='\033[0;32m'; YEL='\033[0;33m'; RED='\033[0;31m'; CYA='\033[0;36m'; NC='\033[0m'
big_hr() { echo "============================================================"; }

# ---- 环境检查：C 编译器 ----
if   command -v gcc   >/dev/null 2>&1; then CC=gcc
elif command -v cc    >/dev/null 2>&1; then CC=cc
elif command -v clang >/dev/null 2>&1; then CC=clang
else
  echo -e "${RED}✗ 未检测到 C 编译器（gcc / cc / clang）。${NC}"
  echo
  echo "  请先安装："
  echo "    macOS         →  xcode-select --install"
  echo "    Ubuntu/Debian →  sudo apt install build-essential"
  echo
  read -rsp "按任意键退出..."; echo; exit 1
fi
HAVE_MAKE=$(command -v make >/dev/null 2>&1 && echo yes || echo no)

# ---- 编译 ----
build() {
  echo -e "${CYA}>> 正在编译 ...${NC}"
  mkdir -p build
  if [ "$HAVE_MAKE" = yes ] && make all >/dev/null 2>&1; then
    :
  else
    "$CC" -std=c11 -Wall -O2 -Iinclude -o build/social_network src/*.c -lm || { echo -e "${RED}✗ 编译失败。${NC}"; return 1; }
  fi
  [ -x build/social_network ] || { echo -e "${RED}✗ 编译失败。${NC}"; return 1; }
  echo -e "${GRN}✓ 构建完成：build/social_network${NC}"
}

# ---- 自动演示（8 用户示例，对照报告第 4 节）----
run_demo() {
  build || return 1
  echo
  echo -e "${CYA}>> 自动演示：8 用户示例会话（需求文档表 1）${NC}"
  big_hr
  ./build/social_network data/users8.txt < examples/session8.txt
  big_hr
  echo -e "${GRN}（完整输出转录见 examples/session8.out.txt）${NC}"
}

# ---- 交互式体验（20 用户）----
run_interactive() {
  build || return 1
  echo
  echo "============================================================"
  echo "  交互模式（20 用户数据）。菜单为中文编号，退出请在菜单选 0。"
  echo "============================================================"
  ./build/social_network data/users20.txt
}

# ---- 全部测试 ----
run_tests() {
  if [ "$HAVE_MAKE" != yes ]; then
    echo -e "${YEL}! 未检测到 make，跳过自动化测试（可运行测试：连接各模块后 make test）。${NC}"
    return 0
  fi
  build || return 1
  echo
  make test
}

# ---- 主菜单 ----
while true; do
  clear 2>/dev/null || true
  big_hr
  echo -e "        ${CYA}小型社交网络系统 —— 一键运行${NC}"
  echo "        （数据结构课程设计 · 202532110113 刘亦轩）"
  big_hr
  echo "  [1] 自动演示：回放 8 用户示例会话（对照报告第 4 节）"
  echo "  [2] 交互式体验：手动操作菜单（20 用户数据）"
  echo "  [3] 运行全部测试（make test）"
  echo "  [0] 退出"
  echo
  read -rp "请输入编号 [0-3]：" choice || exit 0
  case "$choice" in
    1) run_demo ;;
    2) run_interactive ;;
    3) run_tests ;;
    0) echo "再见！"; exit 0 ;;
    *) echo -e "${RED}无效输入，请重新选择。${NC}"; sleep 1 ;;
  esac
  echo
  read -rsp "按任意键继续..."; echo
done