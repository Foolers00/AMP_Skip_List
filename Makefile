.PHONY: all clean
CC = gcc -g -Wall


all: main.o skip_list_seq.o skip_list_lock.o skip_list_lockf.o
	$(CC) -fopenmp -o prog main.o skip_list_seq.o


main.o: main.c skip_list_seq.h skip_list_lock.h
	$(CC) -fopenmp -c  main.c


skip_list_seq.o: skip_list_seq.c skip_list_seq.h
	$(CC) -c skip_list_seq.c

skip_list_lock.o: skip_list_lock.c skip_list_lock.h
	$(CC) -fopenmp -c skip_list_lock.c

skip_list_lockf.o: skip_list_lockf.c skip_list_lockf.h
	$(CC) -c skip_list_lockf.c

clean:
	rm -rf prog main.o skip_list_seq.o skip_list_lock.o skip_list_lockf.o
