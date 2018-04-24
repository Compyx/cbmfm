# vim: set noet ts=8 sw=8 sts=8:
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

LIB_SRCS = src/lib/base/io.c \
	   src/lib/base/errors.c \
	   src/lib/base/mem.c \
	   src/lib/base/image.c \
	   src/lib/base/log.c \
	   src/lib/base/file.c \
	   src/lib/image/ark.c \
	   src/lib/base/dir.c \
	   src/lib/base/petasc.c \
	   src/lib/base/dxx.c \
	   src/lib/image/d64.c \
	   src/lib/image/t64.c \
	   src/lib/image/lnx.c \
	   src/lib/image/detect.c \
	   src/lib/base/dirent.c

GUI_SRCS = src/gui/main.c

TEST_SRCS = src/tests/testcase.c \
	    src/tests/test_lib_base.c \
	    src/tests/test_lib_base_dxx.c \
	    src/tests/test_lib_base_dir.c \
	    src/tests/test_lib_image_ark.c \
	    src/tests/test_lib_image_d64.c \
	    src/tests/test_lib_image_t64.c \
	    src/tests/test_lob_image_lnx.c

HEADERS = 

STATIC_LIB = libcbmfm.a

TESTER = test-runner
TESTER_OBJS = test-runner.o \
	      testcase.o \
	      test_lib_base.o \
	      test_lib_base_dxx.o \
	      test_lib_image_ark.o \
	      test_lib_image_d64.o \
	      test_lib_base_dir.o \
	      test_lib_image_t64.o \
	      test_lib_image_lnx.o

GUI = cbmfm

LIB_OBJS = $(LIB_SRCS:.c=.o)
GUI_OBJS = $(GUI_SRCS:.c=.og)

all: test-runner $(STATIC_LIB) $(GUI) $(TESTER)


$(STATIC_LIB): $(LIB_OBJS) $(HEADERS)
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


# Dependencies of the various objects, according to headers included

# Dependencies of objects in src/lib/base

src/lib/base/dir.o: \
	src/lib/base/dirent.o \
	src/lib/base/errors.o \
	src/lib/base/file.o \
	src/lib/base/mem.o \
	src/lib/base/petasc.o
src/lib/base/dxx.o: \
	src/lib/base/dirent.o \
	src/lib/base/errors.o \
	src/lib/base/mem.o \
	src/lib/base/image.o
src/lib/base/errors.o:
src/lib/base/file.o: \
	src/lib/base/image.o \
	src/lib/base/io.o \
	src/lib/base/log.o \
	src/lib/base/mem.o \
	src/lib/base/petasc.o
src/lib/base/image.o: \
	src/lib/base/mem.o \
	src/lib/base/io.o
src/lib/base/io.o: \
	src/lib/base/errors.o \
	src/lib/base/mem.o
src/lib/base/log.o: \
	src/lib/base/errors.o
src/lib/base/mem.o: \
	src/lib/base/errors.o
src/lib/base/petasc.o:

# Dependencies of objects in src/lib/image

src/lib/image/ark.o: \
	src/lib/base/dir.o \
	src/lib/base/dirent.o \
	src/lib/base/errors.o \
	src/lib/base/file.o \
	src/lib/base/image.o \
	src/lib/base/io.o \
	src/lib/base/mem.o
src/lib/image/d64.o: \
	src/lib/base/dir.o \
	src/lib/base/dxx.o \
	src/lib/base/errors.o \
	src/lib/base/file.o \
	src/lib/base/image.o \
	src/lib/base/io.o \
	src/lib/base/log.o \
	src/lib/base/mem.o \
	src/lib/base/petasc.o
src/lib/image/detect.o: \
	src/lib/base/log.o \
	src/lib/image/ark.o \
	src/lib/image/d64.o \
	src/lib/image/lnx.o \
	src/lib/image/t64.o
src/lib/image/lnx.o: \
	src/lib/base/dir.o \
	src/lib/base/dirent.o \
	src/lib/base/errors.o \
	src/lib/base/file.o \
	src/lib/base/image.o \
	src/lib/base/io.o \
	src/lib/base/log.o \
	src/lib/base/mem.o
src/lib/image/t64.o: \
	src/lib/base/dir.o \
	src/lib/base/dirent.o \
	src/lib/base/errors.o \
	src/lib/base/file.o \
	src/lib/base/image.o \
	src/lib/base/io.o \
	src/lib/base/log.o \
	src/lib/base/mem.o


.PHONY: clean
clean:
	rm -f *.o src/*.o src/lib/*.o src/lib/base/*.o src/lib/image/*.o \
	    src/gui/*.og
	rm -f $(TESTER) $(STATIC_LIB) $(GUI)
	rm -f *.sid
	rm -f *.del
	rm -f *.seq
	rm -f *.prg
	rm -f *.usr
	rm -f *.rel

.PHONY: doc
doc:
	doxygen 1>/dev/null

.PHONY: distclean
distclean: clean
	rm -rfd doc/doxygen/*
