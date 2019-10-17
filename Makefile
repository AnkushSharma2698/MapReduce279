CC = g++
CFLAGS = -Wall -std=c++11

wc: compile
	$(CC) $(CFLAGS) -o wordcount distwc.cc threadpool.o mapreduce.o

compile:
	$(CC) $(CFLAGS) -c threadpool.cc
	$(CC) $(CFLAGS) -c mapreduce.cc
clean:
	rm *.o
	rm wc

compress:
	tar -czf mapreduce.tar.gz distwc.cc mapreduce.cc threadpool.cc Makefile 
