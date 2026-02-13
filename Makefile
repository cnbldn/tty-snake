default:
	gcc src/main.c src/coordQueue.c src/snake.c -lncursesw

run:
	gcc src/main.c src/coordQueue.c src/snake.c -lncursesw && ./a.out

