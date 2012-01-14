CC=gcc
EDL=gcc
RM=rm
BUILDDIR=build/
SRCDIR=src/
CCFLAGS=-g -O0 -Wall
LDFLAGS=
RMFLAGS=-f
EXE=CofiTyHome
LIBS=-lpthread -lrt -ljson
EFFACE=clean
OBJ=$(BUILDDIR)guiNetwork.o $(BUILDDIR)sensorsNetwork.o $(BUILDDIR)guiInterface.o

$(EXE) : $(OBJ) $(BUILDDIR)main.o
		$(EDL) $(LDFLAGS) -o $(EXE) $(OBJ) $(LIBS) $(BUILDDIR)main.o

$(BUILDDIR)%.o : $(SRCDIR)%.c $(SRCDIR)%.h
		$(CC) $(CCFLAGS) -c $< -o $@

$(BUILDDIR)main.o : $(SRCDIR)main.c
		$(CC) $(CCFLAGS) -c $< -o $@

$(EFFACE) :
		$(RM) $(RMFLAGS) $(BUILDDIR)*.o $(EXE) core
