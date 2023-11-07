# this is makefile for MeMS

all: clean example check

example: example.c mems.h
	gcc -o example.out example.c

check: check.c mems.h
	gcc -o check.out check.c
	
clean:
	rm -rf example check