.PHONY: all clean

all: main.o skip_list_seq.o
	gcc -o prog main.o skip_list_seq.o


main.o: main.c skip_list_seq.h 
	gcc -c main.c


skip_list_seq.o: skip_list_seq.c skip_list_seq.h
	gcc -c skip_list_seq.c


clean:
	rm -rf prog main.o skip_list_seq.o
