CC = cc 
SRC = ./json

main: $(SRC)/*.h main.c
	$(CC) main.c -o main -ggdb2