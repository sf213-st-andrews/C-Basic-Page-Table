CC	= gcc

all: starter check experiment

starter:
	$(CC) -std=c99 -o test paging.c test.c

check:
	$(CC) -std=c99 -o check paging.c check.c

experiment:
	$(CC) -std=c99 -o experiment paging.c experiment.c

clean:
	rm -f *.o test check experiment
