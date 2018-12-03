CC=gcc  
CFLAGS= -g -c -Wall -O0
OBJ = main.o funs.o sorting.o trie.o extras.o
LIBS= -lm
OUT  = minisearch

$(OUT): $(OBJ)
	$(CC) -g $(OBJ) -o $@ $(LIBS) 

main.o: main.c
	$(CC) $(CFLAGS) main.c

funs.o: funs.c
	$(CC) $(CFLAGS) funs.c

sorting.o: sorting.c
	$(CC) $(CFLAGS) sorting.c

trie.o: trie.c
	$(CC) $(CFLAGS) trie.c

extras.o: extras.c
	$(CC) $(CFLAGS) extras.c	

clean:
	rm -f $(OBJ) $(OUT) 
