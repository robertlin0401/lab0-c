#include <stdio.h>
#include <stdlib.h>
#include <math.h>

FILE *fp = NULL;
int num = 5;

static void printString(int index)
{
    for (int iter = num - 1; iter >= 0; --iter) {
        int id = (index / (int)pow(26, iter)) % 26;
        fprintf(fp, "%c", id + 97);
    }
}

static void printCommand()
{
    int count = (int)pow(26, num);
    for (int iter = count - 1; iter >= 0; --iter) {
        fprintf(fp, "ih ");
        printString(iter);
        fprintf(fp, "\n");
    }
}

int main(int argc, char *argv[]) {
    fp = fopen("traces/test.cmd", "w");
    if (fp == NULL)
        return -1;
    if (argc == 2)
        num = atoi(argv[1]);

    fprintf(fp, "new\n");
    printCommand();
    fprintf(fp, "sort\n");
    fprintf(fp, "free\n");
    printf("finished\n");
    return 0;
}

