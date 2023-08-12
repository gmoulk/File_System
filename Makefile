all: FileSystem

FileSystem: main.c
	gcc -g -Wall main.c -o FileSystem

clean:
	rm -f FileSystem