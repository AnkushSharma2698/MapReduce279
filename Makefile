CC = g++
CFLAGS = -Wall -std=c++11

wc: compile
	$(CC) $(CFLAGS) -o wordcount distwc.cc threadpool.o mapreduce.o -lpthread

compile:
	$(CC) $(CFLAGS) -c threadpool.cc -lpthread
	$(CC) $(CFLAGS) -c mapreduce.cc -lpthread
clean:
	rm *.o
	rm wc

compress:
	tar -czf mapreduce.tar.gz distwc.cc mapreduce.cc threadpool.cc mapreduce.hpp threadpool.hpp wordcount Makefile readme.md
