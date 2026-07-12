# Source files
SHELL := C:/msys64/usr/bin/bash.exe
CC = gcc

SRC = *.c FrontEnd/*.c
OUT = myopl.exe

CFLAGS = -Wall -std=c99
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
all: build run
build:
	$(CC)   $(SRC)  -o $(OUT) 

run:
	./$(OUT)
check:
	$(CC) -g $(SRC) -o $(OUT) 
	gdb  ./$(OUT)

.PHONY : clean
clean:
#	del /q *.o *.exe FrontEnd/*.o  2>nul
	rm -f *.o *.exe FrontEnd/*.o

	
