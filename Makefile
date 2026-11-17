CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

ifeq ($(OS),Windows_NT)
MSYS2_ROOT ?= C:/msys64
CC = $(MSYS2_ROOT)/ucrt64/bin/gcc.exe
SHELL := $(MSYS2_ROOT)/usr/bin/sh.exe
CPPFLAGS += -I"$(MSYS2_ROOT)/ucrt64/include/ncurses"
LDLIBS += -lncursesw
TARGET = betflip.exe
export TERMINFO := $(MSYS2_ROOT)/ucrt64/lib/terminfo
else
LDLIBS += -lncurses
TARGET = betflip
endif

.PHONY: all run clean
# Keep partial build output on error; deleting files requires explicit approval.

all: $(TARGET)

$(TARGET): game.c Makefile
	"$(CC)" $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) "game.c" -o "$@" $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
