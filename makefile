ppd: ppd.c
	gcc -std=gnu99 -Wall -o ppd ppd.c -lm -lxdo -lpulse-simple -lpulse

run: ppd
	./ppd

.PHONY: run
