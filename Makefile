# Makefile for CS 453
# compile binary from multiple source files

SRC = main.cpp sound.cpp
BIN = main

CC = g++
CPPFLAGS = -O2 -W -Wall -g
LDLIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lpng -lz
SOUNDLIBS = -lmpg123 -lao -ldl -lm
OBJ = $(SRC:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDLIBS) $(SOUNDLIBS)

clean:
	rm -f $(BIN) $(OBJ) core*
