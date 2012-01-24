CC=gcc
EDL=gcc
RM=rm
BUILDDIR=build/
SRCDIR=src/
CCFLAGS=-g -O0 -Wall
LDFLAGS=
RMFLAGS=-f
EXE=CofiTyHome
LIBS=-lpthread -lrt -Llib -ljson -lfl
EFFACE=clean
OBJ=$(BUILDDIR)guiNetwork.o $(BUILDDIR)sensorsNetwork.o $(BUILDDIR)guiInterface.o $(BUILDDIR)actions.o $(BUILDDIR)sensors.o 

FLEX=flex -o $(SRCDIR)lex.yy.c
BISON=bison -o $(SRCDIR)rules.tab.c -d

$(EXE) : flexBison $(OBJ) $(BUILDDIR)main.o
		$(EDL) $(LDFLAGS) -o $(EXE) $(OBJ) $(LIBS) $(SRCDIR)lex.yy.c $(SRCDIR)rules.tab.c $(BUILDDIR)main.o

$(BUILDDIR)%.o : $(SRCDIR)%.c $(SRCDIR)%.h
		$(CC) $(CCFLAGS) -c $< -o $@

$(BUILDDIR)main.o : $(SRCDIR)main.c
		$(CC) $(CCFLAGS) -c $< -o $@

$(EFFACE) :
		$(RM) $(RMFLAGS) $(BUILDDIR)*.o $(EXE) core
		
flexBison : $(SRCDIR)lex.yy.c $(SRCDIR)rules.tab.c
		
$(SRCDIR)lex.yy.c : $(SRCDIR)rules.l
		$(FLEX) $<
		
$(SRCDIR)rules.tab.c : $(SRCDIR)rules.y
		$(BISON) $< 

jonas:
	bison -d -o $(SRCDIR)rules.tab.c $(SRCDIR)rules.y 
	flex  -o $(SRCDIR)lex.yy.c $(SRCDIR)rules.l
	gcc -o sortieTst $(SRCDIR)*.c -lpthread -lrt -Llib -ljson -lfl -w
