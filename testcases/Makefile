# this is makefile for testcases

all: clean merge_holes

merge_holes: merge_holes.c ../mems.h
	gcc -o merge_holes.out merge_holes.c

clean:
	rm -rf *.out