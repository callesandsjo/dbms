SRC := src
OBJ := objects

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

FLAGS := -pthread -fPIC


all: $(OBJECTS)
	gcc $(FLAGS) $^ lib/librequest.so -o db_server

$(OBJ)/%.o: $(SRC)/%.c
	gcc -I$(SRC) -c $< -o $@

clean:
	rm -f objects/*
	rm -f db_server