CC=gcc
CFLAGS=-O3 -g
EXEC=louvain

all: $(EXEC)

louvain: bipartition.o bilouvain.o
	$(CC) -o bilouvain bipartition.o bilouvain.o $(CFLAGS)
	g++ SBM_gen.cpp -o gensbm
	g++ expe.c -o expe
clean:
	rm *.o bilouvain gensbm expe

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
