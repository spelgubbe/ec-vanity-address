CXXPATH = cxxtest-4.4

CXX = g++ -std=c++17 -g -O3

objects = secp256k1.o blockmath.o

main:
	gcc -c -Ofast -maes -march=native aes-stream/src/aes-stream.c -o aes-stream.o
	g++ -o main main.cpp aes-stream.o -O3 -std=c++17

test: secp256k1_test.cpp $(objects)
	python3 $(CXXPATH)/bin/cxxtestgen --error-printer -o runner.cpp secp256k1_test.cpp
	g++ -o secp256k1_test runner.cpp $(objects) -I$(CXXPATH) $(CFLAGS)

clean:
	rm -f *.o
	rm -f main
	rm -f runner.cpp
	rm -f secp256k1_test

all:
	make test
