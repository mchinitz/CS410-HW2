HW2: HW2.o
	gcc HW2.o -o HW2
HW2.o: HW2.c
	gcc -c HW2.c -std=c99
clean:
	rm .*o