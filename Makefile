all: FileSystem

FileSystem: main.c workers.c
	gcc -g main.c workers.c -o FileSystem -lm -lpthread

clean:
	rm -f FileSystem