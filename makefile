BIN = bin
SRC = src
GCC = gcc

all: createbin client server

createbin:
	@[ -d $(BIN) ] || mkdir -p $(BIN)

client:
	$(GCC) $(SRC)/client/client.c -o $(BIN)/client

server:
	$(GCC) $(SRC)/server/server.c -o $(BIN)/server -lpthread