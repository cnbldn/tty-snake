default:
	gcc src/main.c src/coordQueue.c src/snake.c -lncurses

run:
	gcc src/main.c src/coordQueue.c src/snake.c -lncurses && ./a.out

