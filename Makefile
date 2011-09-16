CPP=g++
CFLAGS=-Wall -O2

_OBJS = elf.o rop.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : rop_find

%.o : %.c %.h
	$(CPP) -c -o $@ $< $(CFLAGS)

src/main.o : $(SRCDIR)/main.c
	$(CPP) -c -o $(SRCDIR)/main.o $(SRCDIR)/main.c $(CFLAGS)

rop_find : src/main.o $(OBJS)
	$(CPP) -o rop_tools src/main.o -ldisasm $(OBJS) $(CFLAGS) -ludis86

clean :
	rm -f src/*.o
