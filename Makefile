# vim: set noet ts=8 :
#
# Makefile
#
#

VPATH = src:src/lib:src/lib/base:src/lib/image:src/gui:src/tests
CC = gcc
LD = gcc

INSTALL_PREFIX=/usr/local

CFLAGS = -Wall -Wextra -pedantic -std=c99 -Wshadow -Wpointer-arith \
	 -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes \
	 -Wswitch -Wswitch-default -Wuninitialized -Wconversion \
	 -Wredundant-decls -Wnested-externs -Wunreachable-code \
	 -O3 -g -Isrc -Isrc/lib -Isrc/lib/base -Isrc/lib/iamge -Isrc/gui \
	 -Isrc/tests -DCBMFM_HOST_UNIX

LIB_SRCS = src/lib/base/io.c src/lib/base/errors.c src/lib/base/mem.c \
	   src/lib/base/image.c src/lib/log.c src/lib/image/ark.c \
	   src/lib/base/dir.c src/lib/base/petasc.c
GUI_SRCS = src/gui/main.c
TEST_SRCS = src/tests/testcase.c src/tests/test-lib-base.c \
	    src/tests/test-lib-image-ark.c

HEADERS = src/lib.h src/lib/base.h src/lib/cbmfm_types.h

STATIC_LIB = libcbmfm.a

TESTER = test-runner
TESTER_OBJS = test-runner.o testcase.o test-lib-base.o test-lib-image-ark.o
GUI = cbmfm

LIB_OBJS = $(LIB_SRCS:.c=.o)
GUI_OBJS = $(GUI_SRCS:.c=.og)

all: test-runner $(STATIC_LIB) $(GUI) $(TESTER)


$(STATIC_LIB): $(LIB_OBJS)
	ar cr ${STATIC_LIB} $^
	ranlib ${STATIC_LIB}

$(TESTER): $(TESTER_OBJS) $(HEADERS) $(STATIC_LIB)
	$(LD) -o $(TESTER) $^

$(GUI): $(GUI_OBJS) $(STATIC_LIB)
	$(CC) $(LDFLAGS) `pkg-config --libs gtk+-3.0` -o $(GUI) $^

# .og files are object files for the Gtk3 GUI
.SUFFIXES: .og

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.c.og:
	$(CC) $(CFLAGS) -Wno-unused-parameter `pkg-config --cflags gtk+-3.0` -c $< -o $@


all: $(TESTER) $(STATIC_LIB)

# dependencies of the various objects, according to headers included
errors.o:
io.o: errors.o
mem.o: errors.o


.PHONY: clean
clean:
	rm -f *.o src/*.o src/lib/*.o src/lib/base/*.o src/lib/gui/*.og
	rm -f $(TESTER) $(STATIC_LIB) $(GUI)
	rm -f *.sid
	rm -f *.prg

.PHONY: doc
doc:
	doxygen 1>/dev/null

.PHONY: distclean
distclean: clean
	rm -rfd doc/html/*
