BIN := bin
OBJ := obj
SRC := src
CC := gcc
FLAGS := -Wall
# FLAGS := -Wall -D DEBUG_SEND_FILE
# FLAGS := -Wall -D DEBUG_SEND_FILE -D DEBUG_BZIP2
# FLAGS := -Wall -D DEBUG_BZIP2
# FLAGS := -Wall -D DEBUG_BZIP2 -D TEST

all: server client

client: bin bwt m2f rle huffman bzip2 stream
	$(CC) $(FLAGS) -o $(BIN)/client $(SRC)/client/client.c $(OBJ)/stream.o $(OBJ)/bwt/bwt.o $(OBJ)/m2f/m2f.o $(OBJ)/rle/rle.o $(OBJ)/huffman/node.o $(OBJ)/huffman/list.o $(OBJ)/huffman/huff.o $(OBJ)/huffman/count.o $(OBJ)/bzip2/bzip2.o -lm

server: bin bwt m2f rle huffman bzip2 stream process $(OBJ)/server/server.o
	$(CC) $(FLAGS) -o $(BIN)/server $(OBJ)/server/server.o $(OBJ)/server/process.o $(OBJ)/stream.o $(OBJ)/bwt/bwt.o $(OBJ)/m2f/m2f.o $(OBJ)/rle/rle.o $(OBJ)/huffman/node.o $(OBJ)/huffman/list.o $(OBJ)/huffman/huff.o $(OBJ)/huffman/count.o $(OBJ)/bzip2/bzip2.o -lpthread -lm


bwt: objBWT
	$(CC) $(FLAGS) -o $(OBJ)/bwt/bwt.o -c $(SRC)/shared/bwt/bwt.c
	
m2f: objM2F
	$(CC) $(FLAGS) -o $(OBJ)/m2f/m2f.o -c $(SRC)/shared/m2f/m2f.c

rle: objRLE
	$(CC) $(FLAGS) -o $(OBJ)/rle/rle.o -c $(SRC)/shared/rle/rle.c

huffman : objHuffman $(OBJ)/huffman/node.o $(OBJ)/huffman/list.o $(OBJ)/huffman/count.o
	$(CC) $(FLAGS) -o $(OBJ)/huffman/huff.o -c $(SRC)/shared/huffman/huff.c

$(OBJ)/huffman/node.o : $(SRC)/shared/huffman/node.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/node.o -c $(SRC)/shared/huffman/node.c

$(OBJ)/huffman/list.o : $(SRC)/shared/huffman/list.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/list.o -c $(SRC)/shared/huffman/list.c

$(OBJ)/huffman/count.o : $(SRC)/shared/huffman/count.c
	$(CC) $(FLAGS) -o $(OBJ)/huffman/count.o -c $(SRC)/shared/huffman/count.c

bzip2: objBZIP2
	$(CC) $(FLAGS) -o $(OBJ)/bzip2/bzip2.o -c $(SRC)/shared/bzip2/bzip2.c

stream:
	$(CC) $(FLAGS) -o $(OBJ)/stream.o -c $(SRC)/shared/stream.c

process: objPROCESS
	$(CC) $(FLAGS) -o $(OBJ)/server/process.o -c $(SRC)/server/process.c

$(OBJ)/server/server.o : $(SRC)/server/server.c
	$(CC) $(FLAGS) -o $(OBJ)/server/server.o -c $(SRC)/server/server.c

bin:
	@[ -d $(BIN) ] || mkdir -p $(BIN)

obj:
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)

objBWT: obj
	@[ -d $(OBJ)/bwt ] || mkdir -p $(OBJ)/bwt
objM2F: obj
	@[ -d $(OBJ)/m2f ] || mkdir -p $(OBJ)/m2f
objRLE: obj
	@[ -d $(OBJ)/rle ] || mkdir -p $(OBJ)/rle
objHuffman: obj
	@[ -d $(OBJ)/huffman ] || mkdir -p $(OBJ)/huffman
objBZIP2: obj
	@[ -d $(OBJ)/bzip2 ] || mkdir -p $(OBJ)/bzip2
objPROCESS: obj
	@[ -d $(OBJ)/server ] || mkdir -p $(OBJ)/server

rebuild: clean all

clean:
	rm -rf obj
	rm -rf bin