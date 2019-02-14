#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAXCHAR 1000

int f(int *lookupTable, int c1, int c2, int c3);
char **str_split(char *a_str, const char a_delim);
int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(int **cells, char *filename);
int mod(int x, int n);
void printMatrix(int *mat, int w, int h);
void printArray(int *arr, int size);
void runIterations(int i, int *lookupTable, const int lookupSize, int *cells, const int cellsSize, int saveToFile);
void writeToFile(char *fileName, int *history, int h, int w);

int main(int argc, char *argv[])
{
    int sz = 1024;
    int *cnfg = malloc(sz*sz*sizeof(int));

    for(size_t i = 0; i < sz; i++)
    {
        for(size_t j = 0; j < sz; j++)
        {
            *(cnfg + sz*i + j) = mod(rand(), 2);
        }
    }
    printMatrix(cnfg, sz, sz);
    return 0;
}

int mod(int x, int m)
{
    return (x % m + m) % m;
}

void printMatrix(int *mat, int w, int h)
{
    for (int i = 0; i < h; i++)
    {
        printArray(mat + i * w, w);
    }
}

void printArray(int *arr, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%d,", arr[i]);
    }
    printf("\n");
}