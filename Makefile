.PHONY: all clean

all: main.o skip_list.o
	gcc -o prog main.o skip_list.o


main.o: main.c skip_list.h 
	gcc -c main.c


skip_list.o: skip_list.c skip_list.h
	gcc -c skip_list.c


clean:
	rm -rf prog main.o skip_list.o