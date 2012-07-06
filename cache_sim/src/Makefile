# variables
CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS= 

all: cachesim

cachesim: cache_functions.o get_settings.o main.o 
	$(CC) cache_functions.o get_settings.o main.o  $(LDFLAGS)  -o ../cachesim

cache_functions.o:  cache_functions.c cache_functions.h data_structures.h defines.h
	$(CC) $(CFLAGS) cache_functions.c

get_settings.o: get_settings.c get_settings.h data_structures.h defines.h
	$(CC) $(CFLAGS) get_settings.c

main.o: main.c cache_functions.h  data_structures.h  defines.h  get_settings.h  main.h
	$(CC) $(CFLAGS) main.c 


clean:
	rm -f ./*.o 
	rm -f ../cachesim
