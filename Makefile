# Makefile for CS 453
# compile binary from multiple source files

SRC = main.cpp
BIN = main

CC = g++
CPPFLAGS = -O2 -W -Wall -g
LDLIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lpng -lz
OBJ = $(SRC:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDLIBS)

clean:
	rm -f $(BIN) $(OBJ) core*
