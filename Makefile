default:
	gcc src/main.c -lncurses

run: src/main.c
	gcc src/main.c -lncurses && ./a.out
