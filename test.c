#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int start = 5;
    int end = 26;

    fp = fopen("test.c", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(i>=start){
            printf("Retrieved line of length %zu:\n", read);
            printf("%s", line);
        }
        if(i >= end){
            break;
        }
        i++;
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}