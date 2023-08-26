all: FileSystem

FileSystem: main.c
	gcc -g main.c -o FileSystem -lm -lpthread

clean:
	rm -f FileSystem