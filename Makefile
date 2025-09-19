CC=gcc
CFLAGS=-Iinclude -Wall
LDFLAGS=-ljpeg -lpng -lm

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
EXEC=imagetool

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f src/*.o $(EXEC)

