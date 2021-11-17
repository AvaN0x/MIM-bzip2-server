BIN := bin
OBJ := obj
SRC := src
GCC := gcc
FLAGS := -Wall

# Section HUFFMAN
HUFFMAN := src/shared/huffman

all: bin obj client server



bin: binHuffman
	@[ -d $(BIN) ] || mkdir -p $(BIN)

binHuffman:
	@[ -d $(BIN)/huffman ] || mkdir -p $(BIN)/huffman



obj: objHuffman
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)

objHuffman:
	@[ -d $(OBJ)/huffman ] || mkdir -p $(OBJ)/huffman



client:
	$(GCC) $(FLAGS) $(SRC)/client/client.c -o $(BIN)/client

server: huffman
	$(GCC) $(FLAGS) -o $(BIN)/server $(SRC)/server/server.c $(HUFFMAN)/node.c $(HUFFMAN)/list.c $(HUFFMAN)/huff.c -lpthread


# Compile everything for Huffman
huffman : bin obj $(OBJ)/huffman/node.o $(OBJ)/huffman/list.o
	$(CC) $(FLAGS) -o $(OBJ)/huffman/huff.o -c $(HUFFMAN)/huff.c

$(OBJ)/huffman/node.o : obj $(HUFFMAN)/node.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/node.o -c $(HUFFMAN)/node.c

$(OBJ)/huffman/list.o : obj $(HUFFMAN)/list.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/list.o -c $(HUFFMAN)/list.c


rebuild: clean all

clean:
	rm -rf obj
	rm -rf bin