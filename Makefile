CC=gcc
CFLAGS= -Wall -O2 -g
LIBS = -ludis86 -llua5.1

_OBJS = elf.o rop.o lua.o strings.o types.o aux.o lua_types.o lua_elf.o \
        lua_dis.o pe.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : rop_find pe_test

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

src/main.o : $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c -o $(SRCDIR)/main.o $(SRCDIR)/main.c

rop_find : src/main.o $(OBJS)
	$(CC) $(CFLAGS) -o rop_tools src/main.o $(OBJS) $(LIBS)

pe_test : tests/pe.c $(OBJS)
	$(CC) $(CFLAGS) -o pe_test tests/pe.c $(OBJS) $(LIBS)

clean :
	rm -f src/*.o
	rm -f rop_tools
	rm -f pe_test
