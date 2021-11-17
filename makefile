BIN := bin
SRC := src
GCC := gcc
FLAGS := -Wall

all: createbin client server

createbin:
	@[ -d $(BIN) ] || mkdir -p $(BIN)

client:
	$(GCC) $(FLAGS) $(SRC)/client/client.c -o $(BIN)/client

server:
	$(GCC) $(FLAGS) $(SRC)/server/server.c -o $(BIN)/server -lpthread