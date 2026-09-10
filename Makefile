# 小型社交网络系统 — 构建脚本
# 用法：make [all|app|test|demo|gen|clean]

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -g
INC     := -Iinclude -Itests
LDLIBS  := -lm

SRC     := $(wildcard src/*.c)
OBJ     := $(patsubst src/%.c,build/obj/%.o,$(SRC))
APP     := build/social_network
# 链接除 main.o 外的全部模块（供测试使用）
APP_OBJS := $(filter-out build/obj/main.o,$(OBJ))
HDRS    := $(wildcard include/*.h) $(wildcard tests/*.h)

TEST_SRCS := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c,build/tests/%,$(TEST_SRCS))
GEN_BIN   := build/gen_data

.PHONY: all app test demo gen clean

# 从 src/main.c 是否存在决定是否构建 app（模块合入齐全前自动跳过）
ALL := test
ifneq ($(wildcard src/main.c),)
ALL += app
endif

all: $(ALL)

app: $(APP)

$(APP): $(OBJ) $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -o $@ $(OBJ) $(LDLIBS)

build/obj/%.o: src/%.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# 每个测试为独立可执行文件：源码 + 除 main.o 外的全部模块
build/tests/%: tests/%.c $(APP_OBJS) $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -o $@ $< $(LDLIBS)

test: $(TEST_BINS)
	@fail=0; \
	for t in $(TEST_BINS); do \
	  echo "== $$t =="; ./$$t || fail=1; \
	done; \
	if [ $$fail -ne 0 ]; then echo "==> TESTS FAILED"; exit 1; fi; \
	echo "==> ALL TESTS PASSED"

gen: $(GEN_BIN)

$(GEN_BIN): tools/gen_data.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)
	@echo "gen_data 构建完成：./build/gen_data <用户数> <种子> <输出文件>"

demo: app
	bash examples/demo.sh

clean:
	rm -rf build