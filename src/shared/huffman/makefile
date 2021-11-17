CC := gcc
workspace := 
FLAGS := 

all: huffman

huffman : bin obj $(workspace)obj/node.o $(workspace)obj/list.o $(workspace)obj/huff.o
	$(CC) $(FLAGS) -o $(workspace)bin/huff $(workspace)obj/huff.o $(workspace)obj/node.o $(workspace)obj/list.o

$(workspace)obj/node.o : obj $(workspace)node.c
	$(CC) $(FLAGS) -o $(workspace)obj/node.o -c $(workspace)node.c

$(workspace)obj/list.o : obj $(workspace)list.c
	$(CC) $(FLAGS) -o $(workspace)obj/list.o -c $(workspace)list.c

$(workspace)obj/huff.o : obj $(workspace)huff.c
	$(CC) $(FLAGS) -o $(workspace)obj/huff.o -c $(workspace)huff.c

bin:
ifeq ("$(wildcard $(workspace)bin)", "")
	mkdir $(workspace)bin
endif

obj:
ifeq ("$(wildcard $(workspace)obj)", "")
	mkdir $(workspace)obj
endif

rebuild: clean all

clean:
	rm -rf obj
	rm -rf bin