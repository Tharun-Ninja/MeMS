# this is makefile for testcases

all: clean merge_holes phy_addr

merge_holes: merge_holes.c ../mems.h
	gcc -o merge_holes.out merge_holes.c

phy_addr: phy_addr.c ../mems.h
	gcc -o phy_addr.out phy_addr.c

clean:
	rm -rf *.out