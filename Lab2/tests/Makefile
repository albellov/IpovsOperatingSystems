tests: tests.o
	is
	gcc -o tests tests.o -L. -lsrevert -Wl,-rpath,. -lcunit

tests.o: tests.c
	gcc -c tests.c
clean:
	rm -rf *.
