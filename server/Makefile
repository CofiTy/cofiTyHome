CC=gcc
EDL=gcc
RM=rm
BUILDDIR=build/
SRCDIR=src/
PARSEDIR=parse/
PARSEBUILDDIR=parseBuild/
CCFLAGS=-g -O0 -Wall
LDFLAGS=
RMFLAGS=-f
EXE=CofiTyHome
LIBS=-lpthread -lrt -Llib -ljson
EFFACE=clean
OBJ=$(BUILDDIR)rules.o $(BUILDDIR)guiNetwork.o $(BUILDDIR)sensorsNetwork.o $(BUILDDIR)guiInterface.o $(BUILDDIR)actions.o $(BUILDDIR)sensors.o $(BUILDDIR)actionneurs.o
PARSEOBJ=$(PARSEBUILDDIR)rules.tab.o $(PARSEBUILDDIR)lex.yy.o

FLEX=flex -o$(PARSEDIR)lex.yy.c
BISON=bison -o$(PARSEDIR)rules.tab.c -d

$(EXE) : $(PARSEOBJ) $(OBJ) $(BUILDDIR)main.o
		$(EDL) $(LDFLAGS) -o $(EXE) $(OBJ) $(PARSEOBJ) $(LIBS) $(BUILDDIR)main.o

$(BUILDDIR)%.o : $(SRCDIR)%.c $(SRCDIR)%.h
		$(CC) $(CCFLAGS) -c $< -o $@

$(BUILDDIR)main.o : $(SRCDIR)main.c
		$(CC) $(CCFLAGS) -c $< -o $@

$(PARSEBUILDDIR)%.o : $(PARSEDIR)%.c
		$(CC) $(CCFLAGS) -c $< -o $@

$(PARSEDIR)lex.yy.c : $(PARSEDIR)rules.l
		$(FLEX) $<

$(PARSEDIR)rules.tab.c : $(PARSEDIR)rules.y
		$(BISON) $<

$(EFFACE) :
		$(RM) $(RMFLAGS) $(BUILDDIR)*.o $(EXE) core $(PARSEBUILDIR)*.o $(PARSEDIR)*.c $(PARSEDIR)*.h
