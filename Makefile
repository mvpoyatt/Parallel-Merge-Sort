mergesort: mergesort.o mergesortparallel.o
	gcc -lpthread -Wall mergesort.o mergesortparallel.o -o mergesort

mergesort.o: mergesort.c
	gcc -c -Wall mergesort.c -lpthread

mergesortparallel.o: mergesortparallel.c
	gcc -c -Wall mergesortparallel.c -lpthread

clean:
	rm -f mergesort.o mergesortparallel.o mergesort
