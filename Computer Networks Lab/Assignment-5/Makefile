run: libmsocket.a
	gcc s.c  -o s -L. -lmsocket
	gcc c.c  -o c -L. -lmsocket

libmsocket.a: mysocket.o
	ar -rcs libmsocket.a mysocket.o

mysocket.o: mysocket.c mysocket.h
	gcc -c mysocket.c -o mysocket.o -lpthread

clean:
	rm -f mysocket.o libmsocket.a 