# Compilateur et ses arguments
CC = g++
CFLAGS = -std=c++17 -Wall `pkg-config --cflags opencv4`

# Flags des bibliotheques standard et externe
LIBFLAG = `pkg-config --libs opencv4` -lboost_system -lsfml-graphics -lsfml-window -lsfml-system
LDFLAG = 

# Nom de l'executable final
EXEC = app

# Répertoires du projet
SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build
INC_DIR = inc

###### Fichiers ######
SRCS = $(wildcard $(SRC_DIR)/*.cpp) 
BINS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) 
DEPS = $(OBJS:.o=.d)

TARGET = $(BIN_DIR)/$(EXEC)

all: setup $(TARGET)
	@echo "Création de l'executable terminée."

# Lie les fichiers .o ensemble en un executable avec le compilateur (règle résolue par "all" !)
$(TARGET): $(BINS)
	@echo "Liaison des fichiers .o..."
	$(CC) $(BINS) -o $(TARGET) $(LIBFLAG)
	@echo "Liaison des .o terminée."

# Compile chaque fichier .c $< en un fichier .o $@ (règle résolue par la suivante !)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compilation de $< en $@..."
	$(CC) $(CFLAGS) -c $< -o $@ 
	@echo "Compilation de $< en $@ terminée."

# Créée les dossiers
setup:
	@echo "Création des dossiers du projet..."
	mkdir -p $(BUILD_DIR) $(BIN_DIR) $(SRC_DIR) $(INC_DIR)
	@echo "Création des dossiers terminée."

debug:
	@echo "$(SRCS) : $(BINS)"

# Supprime les fichiers compilés
clean:
	@echo "Nettoyage des fichiers objets..."
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	@echo "Nettoyage terminé."

aclean: clean
	@echo "Nettoyage total..."
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(BIN_DIR)/$(EXEC)
	@echo "Nettoyage total terminé." 

# Recompile
rebuild: aclean all

# Inclusion des dépendances auto-générées
-include $(DEPS)

# Précise quels cibles ne sont jamais des fichiers pour systématiquement exécuter la cible 
.PHONY: all clean aclean rebuild setup

