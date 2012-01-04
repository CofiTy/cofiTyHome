# Choix du compilateur.
CC = gcc
# Options du compilateur (ici on met des options strictes).
CFLAGS = -Wall -Werror -pedantic
# Nom de l'exécutable.
EXEC_NAME = ordo
# Rien à inclure.
INCLUDES = 
# Libraries utilisées.
LIBS = 
# Fichiers à compiler (les fichiers sont ajoutés automatiquement).
SRC = $(wildcard *.c)
OBJ_FILES = $(SRC:.cpp=.o)

# Compilation générale (par défault).
all: $(EXEC_NAME)

# Suppression de tous les fichiers créés (Executable et Objets).
clean:
	rm $(EXEC_NAME) $(OBJ_FILES)

# Linkage pour la création de l'exécutable.
$(EXEC_NAME): $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

# Compilation des fichiers indépendants (Patterns).
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<
