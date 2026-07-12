# Source files
CC = gcc

SRC = *.c FrontEnd/*.c
OUT = myopl.exe

CFLAGS = -Wall -Wextra -std=c99 
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
all: build run
build:
	$(CC) $(CFLAGS)  $(SRC)  -o $(OUT) 

run:
	./$(OUT)
check:
	$(CC) -g $(SRC) -o $(OUT) 
	gdb  ./$(OUT)

.PHONY : clean
clean:
#	del /q *.o *.exe FrontEnd/*.o  2>nul
	rm -f *.o *.exe FrontEnd/*.o

	
