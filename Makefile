CC=gcc
CFLAGS=-ggdb -DDEBUG_LEVEL=4
LDFLAGS=-lm -lgd -lsimpleparser
OBJS=graph.o language.o debug.o

.PHONY: clean

graph: $(OBJS)
	$(CC) $(LDFLAGS) -o graph $(OBJS)

graph.o: graph.c debug.h
	$(CC) $(CFLAGS) -c -o graph.o graph.c

language.o: language.c language.h debug.h
	$(CC) $(CFLAGS) -c -o language.o language.c

debug.o: debug.c
	$(CC) $(CFLAGS) -c -o debug.o debug.c

clean:
	rm -f *~ $(OBJS) graph
