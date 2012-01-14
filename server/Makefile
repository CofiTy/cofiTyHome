CC=gcc
EDL=gcc
RM=rm
BUILDDIR=build/
SRCDIR=src/
KERNEL=$(shell uname -r | cut -d '-' -f1 | cut -d '.' -f3)
ifeq ($(shell expr $(KERNEL) \< 28),1)
	CCFLAGS=-DOLD -g -O0 -Wall -Werror
else
	CCFLAGS=-DNEW -g -O0 -Wall -Werror
endif
LDFLAGS=
RMFLAGS=-f
EXE=CofiTyHome
LIBS=-lpthread
EFFACE=clean
OBJ=$(BUILDDIR)guiNetwork.o $(BUILDDIR)sensorsNetwork.o

$(EXE) : $(OBJ) $(BUILDDIR)main.o
		$(EDL) $(LDFLAGS) -o $(EXE) $(OBJ) $(LIBS) $(BUILDDIR)main.o

$(BUILDDIR)%.o : $(SRCDIR)%.c $(SRCDIR)%.h
		$(CC) $(CCFLAGS) -c $< -o $@

$(BUILDDIR)main.o : $(SRCDIR)main.c
		$(CC) $(CCFLAGS) -c $< -o $@

$(EFFACE) :
		$(RM) $(RMFLAGS) $(BUILDDIR)*.o $(EXE) core
