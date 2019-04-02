CLASSDIR=${PWD}
INCLUDE = -I$(CLASSDIR)/libfdr
LIBS = $(CLASSDIR)/libfdr/libfdr.a
CC = gcc

EX = lsexe
all: $(EX)
clean: 
	rm -f $(EX) *.o

lsexe: lsexe.o $(LIBS)
	$(CC) $(INCLUDE) -o lsexe lsexe.o $(LIBS)

.SUFFIXES: .c .o

.c.o: 
	$(CC) $(INCLUDE) -g  -c  $*.c
