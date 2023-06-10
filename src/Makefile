.PHONY: all clean
CC = gcc -g -Wall


all: main.o skip_list_seq.o skip_list_lock.o skip_list_lockf.o test.o skip_list_lockf_improved.o
	$(CC) -fopenmp -o prog main.o skip_list_seq.o skip_list_lock.o skip_list_lockf.o test.o skip_list_lockf_improved.o


main.o: main.c test.h
	$(CC) -fopenmp -c  main.c


skip_list_seq.o: skip_list_seq.c skip_list_seq.h
	$(CC) -c skip_list_seq.c


skip_list_lock.o: skip_list_lock.c skip_list_lock.h skip_list_seq.h
	$(CC) -fopenmp -c skip_list_lock.c


skip_list_lockf.o: skip_list_lockf.c skip_list_lockf.h
	$(CC) -fopenmp -c skip_list_lockf.c


skip_list_lockf_improved.o: skip_list_lockf_improved.c skip_list_lockf_improved.h
	$(CC) -fopenmp -c skip_list_lockf_improved.c


test.o: test.c test.h skip_list_seq.h skip_list_lock.h skip_list_lockf.h
	$(CC) -fopenmp -c  test.c

clean:
	rm -rf prog *.o
