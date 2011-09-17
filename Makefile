CPP=gcc
CFLAGS= -Wall -O2
LIBS = -ludis86 -llua5.1 -ldisasm

_OBJS = elf.o rop.o lua.o mnemonic_strings.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : rop_find

%.o : %.c %.h
	$(CPP) $(CFLAGS) -c -o $@ $<

src/main.o : $(SRCDIR)/main.c
	$(CPP) $(CFLAGS) -c -o $(SRCDIR)/main.o $(SRCDIR)/main.c

rop_find : src/main.o $(OBJS)
	$(CPP) $(CFLAGS) -o rop_tools src/main.o $(OBJS) $(LIBS)

clean :
	rm -f src/*.o
