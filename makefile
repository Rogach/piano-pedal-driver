ppd: ppd.c
	gcc -std=gnu99 -Wall -o ppd ppd.c -lesd -laudiofile -lm -lxdo

run: ppd
	./ppd

.PHONY: run
