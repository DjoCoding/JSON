CC   = cc 
SRC  = ./lib
JSON = ./json.h
MAIN = main.c


main: $(SRC)/*.h $(MAIN) $(JSON)
	$(CC) $(MAIN) -o main -ggdb2 -Wall