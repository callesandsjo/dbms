SRC := src
OBJ := objects

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

FLAGS := -pthread -fPIC


all: $(OBJECTS)
	gcc $(FLAGS) $^ lib/librequest.so -o db_server

$(OBJ)/%.o: $(SRC)/%.c init
	gcc -I$(SRC) -c $< -o $@

init:
	mkdir -p objects
	mkdir -p database

clean:
	rm -rf objects/
	rm -f db_server