all:
		gcc -I src/include -L src/lib -o main main.c -lmingw32 -lSDL3main -lSDL3