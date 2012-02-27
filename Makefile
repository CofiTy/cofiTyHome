CC=gcc
EDL=gcc
RM=rm
BUILDDIR=server/build/
SRCDIR=server/src/
PARSEDIR=server/parse/
PARSEBUILDDIR=server/parseBuild/
MEMORYDIR=kernel/memory/
MEMORYBUILDDIR=kernel/memoryBuild/
CCFLAGS=-g -O0 -Wall
LDFLAGS=
RMFLAGS=-f
EXE=CofiTyHome
LIBS=-lpthread -lrt -Lserver/lib -ljson -lX11 -lXtst -lXext
EFFACE=clean
OBJ=$(BUILDDIR)rules.o $(BUILDDIR)initCheckRules.o $(BUILDDIR)init.o $(BUILDDIR)guiNetwork.o $(BUILDDIR)sensorsNetwork.o $(BUILDDIR)guiInterface.o $(BUILDDIR)actions.o $(BUILDDIR)sensors.o $(BUILDDIR)actionneurs.o $(BUILDDIR)logWatch.o $(BUILDDIR)timedCheck.o
PARSEOBJ=$(PARSEBUILDDIR)rules.tab.o $(PARSEBUILDDIR)lex.yy.o
MEMORYOBJ=$(MEMORYBUILDDIR)memory.o

FLEX=flex -o$(PARSEDIR)lex.yy.c
BISON=byacc -o$(PARSEDIR)rules.tab.c -d

$(EXE) : $(PARSEOBJ) $(MEMORYOBJ) $(OBJ) $(BUILDDIR)main.o
		$(EDL) $(LDFLAGS) -o $(EXE) $(OBJ) $(PARSEOBJ) $(MEMORYOBJ) $(LIBS) $(BUILDDIR)main.o

$(BUILDDIR)%.o : $(SRCDIR)%.c $(SRCDIR)%.h
		$(CC) $(CCFLAGS) -c $< -o $@
		
$(MEMORYBUILDDIR)%.o : $(MEMORYDIR)%.c $(MEMORYDIR)%.h
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
		$(RM) $(RMFLAGS) $(BUILDDIR)*.o $(EXE) core $(PARSEBUILDIR)*.o $(PARSEDIR)*.c $(PARSEDIR)*.h $(MEMORYBUILDDIR)*.o
