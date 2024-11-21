.PHONY: all clean
all: lab2PPLinux

lab2PPLinux: main.o ThreadPool.o ThreadPool.h
	g++ main.o ThreadPool.o -o lab2PPLinux
main.o: main.cpp ThreadPool.h
	g++ -c main.cpp
ThreadPool.o: ThreadPool.cpp ThreadPool.h
	g++ -c ThreadPool.cpp
clean:
	rm lab2PPLinux *.o *.txt
