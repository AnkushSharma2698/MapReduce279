CC = g++
CFLAGS = -Wall -Werror -std=c++11

wc: compile
	$(CC) $(CFLAGS) -o wordcount distwc.cc threadpool.o mapreduce.o -pthread

compile:
	$(CC) $(CFLAGS) -c threadpool.cc -pthread
	$(CC) $(CFLAGS) -c mapreduce.cc -pthread
clean:
	rm *.o
	rm ./wordcount

compress:
	tar -czf mapreduce.tar.gz distwc.cc mapreduce.cc threadpool.cc mapreduce.h threadpool.h Makefile readme.md
