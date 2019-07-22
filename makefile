CC := gcc

SRC := $(shell find . -type f -name '*.c' ! -path '*/quickjs/*')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

CFLAGS ?= -Wall -MMD \
    -D_FORTIFY_SOURCE=2 \
    -Wextra -Wcast-align -Wcast-qual -Wpointer-arith \
    -Waggregate-return -Wunreachable-code -Wfloat-equal \
    -Wformat=2 -Wredundant-decls -Wundef \
    -Wdisabled-optimization -Wshadow -Wmissing-braces \
    -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wconversion \
    -Wno-unused-parameter \
    -pedantic -std=c11 -O3

CPPFLAGS := -Iquickjs
LDLIBS := -ldl -lm 

all: main

debug: CFLAGS += -g -O0
debug: main

main: test_module.o test.o quickjs/libquickjs.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test_module.c: quickjs/qjsc test.o test_module.js
	./quickjs/qjsc -e -M test.so,test -m -o $@ test_module.js

quickjs/libquickjs.a: 
	make -C quickjs CONFIG_M32=n libquickjs.a

quickjs/qjsc:
	make -C quickjs CONFIG_M32=n qjsc

.PHONY: clean

clean:
	$(RM) *.d *.o		# remove dependency files
	$(RM) test_module.c	# remove main program
	$(RM) main		# remove main program
	make -C quickjs clean	# clean quickjs

-include $(DEP)
