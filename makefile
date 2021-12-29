BIN := bin
OBJ := obj
SRC := src
CC := gcc
# FLAGS := -Wall
# FLAGS := -Wall -D DEBUG_SEND_FILE
FLAGS := -Wall -D DEBUG_SEND_FILE -D DEBUG_BZIP2
# FLAGS := -Wall -D DEBUG_BZIP2
# FLAGS := -Wall -D DEBUG_BZIP2 -D TEST

all: bin obj client server



bin:
	@[ -d $(BIN) ] || mkdir -p $(BIN)


obj: objHuffman objBWT objM2F objRLE objBZIP2
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)

objBWT:
	@[ -d $(OBJ)/bwt ] || mkdir -p $(OBJ)/bwt
objM2F:
	@[ -d $(OBJ)/m2f ] || mkdir -p $(OBJ)/m2f
objRLE:
	@[ -d $(OBJ)/rle ] || mkdir -p $(OBJ)/rle
objHuffman:
	@[ -d $(OBJ)/huffman ] || mkdir -p $(OBJ)/huffman
objBZIP2:
	@[ -d $(OBJ)/bzip2 ] || mkdir -p $(OBJ)/bzip2


client: bwt m2f huffman bzip2
	$(CC) $(FLAGS) -o  $(BIN)/client $(SRC)/shared/stream.c $(SRC)/client/client.c $(SRC)/shared/bwt/bwt.c $(SRC)/shared/m2f/m2f.c $(SRC)/shared/rle/rle.c $(SRC)/shared/huffman/node.c $(SRC)/shared/huffman/list.c $(SRC)/shared/huffman/huff.c $(SRC)/shared/huffman/count.c $(SRC)/shared/bzip2/bzip2.c -lm

server: bwt m2f huffman bzip2
	$(CC) $(FLAGS) -o $(BIN)/server $(SRC)/shared/stream.c $(SRC)/server/process.c $(SRC)/server/server.c $(SRC)/shared/bwt/bwt.c $(SRC)/shared/m2f/m2f.c $(SRC)/shared/rle/rle.c $(SRC)/shared/huffman/node.c $(SRC)/shared/huffman/list.c $(SRC)/shared/huffman/huff.c $(SRC)/shared/huffman/count.c $(SRC)/shared/bzip2/bzip2.c -lpthread -lm


bwt: obj
	$(CC) $(FLAGS) -o $(OBJ)/bwt/bwt.o -c $(SRC)/shared/bwt/bwt.c
	
m2f: obj
	$(CC) $(FLAGS) -o $(OBJ)/m2f/m2f.o -c $(SRC)/shared/m2f/m2f.c

rle: obj
	$(CC) $(FLAGS) -o $(OBJ)/rle/rle.o -c $(SRC)/shared/rle/rle.c


# Compile everything for Huffman
huffman : obj $(OBJ)/huffman/node.o $(OBJ)/huffman/list.o
	$(CC) $(FLAGS) -o $(OBJ)/huffman/huff.o -c $(SRC)/shared/huffman/huff.c

$(OBJ)/huffman/node.o : obj $(SRC)/shared/huffman/node.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/node.o -c $(SRC)/shared/huffman/node.c

$(OBJ)/huffman/list.o : obj $(SRC)/shared/huffman/list.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/list.o -c $(SRC)/shared/huffman/list.c

bzip2: obj
	$(CC) $(FLAGS) -o $(OBJ)/bzip2/bzip2.o -c $(SRC)/shared/bzip2/bzip2.c


rebuild: clean all

clean:
	rm -rf obj
	rm -rf bin
