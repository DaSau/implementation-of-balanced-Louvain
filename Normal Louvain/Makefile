CC=gcc
CFLAGS=-O3 -g
EXEC=louvain

all: $(EXEC)

louvain: partition.o louvain.o
	$(CC) -o louvain partition.o louvain.o $(CFLAGS)
	g++ SBM_gen.cpp -o gensbm
	g++ expe.c -o expe
clean:
	rm *.o louvain gensbm expe

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
