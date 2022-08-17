CC = g++
LFLAGS = -o
CFLAGS = -c -g -Wall -Werror
INC = ./inc
IFLAG = -I$(INC)
BIN = ./bin
OBJ = ./obj
SRC = ./src
SVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valserver 
CVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valclient


all: 	client server 

client: $(OBJ)/Client.o $(OBJ)/ClientMain.o
	$(CC) $(LFLAGS) $(BIN)/client $(OBJ)/Client.o $(OBJ)/ClientMain.o

$(OBJ)/Client.o: $(SRC)/Client.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/Client.cpp 
	mv *.o $(OBJ)

$(OBJ)/ClientMain.o: $(SRC)/ClientMain.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/ClientMain.cpp
	mv *.o $(OBJ)

server: $(OBJ)/Server.o $(OBJ)/ServerMain.o
	$(CC) $(LFLAGS) $(BIN)/server $(OBJ)/Server.o $(OBJ)/ServerMain.o

$(OBJ)/Server.o: $(SRC)/Server.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/Server.cpp 
	mv *.o $(OBJ)

$(OBJ)/ServerMain.o: $(SRC)/ServerMain.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/ServerMain.cpp
	mv *.o $(OBJ)


clean:
	rm -f *.o a.out
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/server $(BIN)/client
	
	@echo "Cleaning Done!"

valgrindserver:
	valgrind $(SVFLAGS) $(BIN)/server 
	mv valserver $(BIN)/

valgrindclient:
	valgrind $(CVFLAGS) $(BIN)/client 
	mv valclient $(BIN)/

