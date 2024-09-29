TARGET		?= rob
BUILD_DIR 	?= ./build

SRCS	= src/main.c src/luadsl.c src/utils.c src/rdir.c
OBJS	= $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS	= $(OBJS:.o=.d)

CC		= gcc -std=gnu99
INCDIRS = -I./libs/include/
CFLAGS 	= -O2 -Wall -Wextra -MD -g
LDFLAGS =
LIBS	= -Llibs/lib -llua -lm

# Default target
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS) libs/lib/liblua.a
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ 

$(BUILD_DIR)/%.o: %.c libs/lib/liblua.a
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(INCDIRS) $(CFLAGS)

# Lua

libs/lib/liblua.a: ./lua-5.4.7/src/liblua.a
	$(MAKE) -C ./lua-5.4.7/ install


./lua-5.4.7/src/liblua.a:
	$(MAKE) -C ./lua-5.4.7/

.PHONY: clean
clean:
	rm -rf libs build rob
	$(MAKE) -C ./lua-5.4.7/ clean


-include $(DEPS)
