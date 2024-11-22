.PHONY: all clean
all: lab2PPLinux

lab2PPLinux: PP\ lab2.o ThreadPool.o ThreadPool.h
	g++ PP\ lab2.o ThreadPool.o -o lab2PPLinux
PP\ lab2.o: PP\ lab2.cpp ThreadPool.h
	g++ -c PP\ lab2.cpp
ThreadPool.o: ThreadPool.cpp ThreadPool.h
	g++ -c ThreadPool.cpp
clean:
	rm lab2PPLinux *.o *.txt
