default:
	gcc src/main.c src/coordQueue.c -lncurses

run:
	gcc src/main.c src/coordQueue.c -lncurses && ./a.out

