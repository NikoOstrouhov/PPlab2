.PHONY: all clean
all: lab3

lab3: main.o ThreadPool.o ThreadPool.h
	g++ main.o ThreadPool.o -o lab3
main.o: main.cpp ThreadPool.h
	g++ -c main.cpp
ThreadPool.o: ThreadPool.cpp ThreadPool.h
	g++ -c ThreadPool.cpp
clean:
	rm lab3 *.o *.txt
