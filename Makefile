# Source files
CC = gcc

SRC = *.c
OUT = game.exe

CFLAGS = -Wall -std=c99
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
all: build run
build:
	$(CC) $(SRC)  FrontEnd/*.c  -o $(OUT) 

run:
	./$(OUT)
check:
	$(CC) -g -o $(OUT) $(SRC)
	gbd ./$(OUT)

	
