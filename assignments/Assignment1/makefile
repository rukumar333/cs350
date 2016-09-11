FLAGS = -g -Wall

shell: main.o JobsList.o
	gcc $(FLAGS) main.o JobsList.o -o cs350sh

main.o: main.c
	gcc $(FLAGS) -c main.c

LinkedList: JobsList.c JobsList.h
	gcc $(FLAGS) -c JobsList.c



