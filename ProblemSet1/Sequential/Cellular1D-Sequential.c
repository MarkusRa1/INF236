#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAXCHAR 1000

int f(int *lookupTable, int c1, int c2, int c3);
char **str_split(char *a_str, const char a_delim);
int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(int **cells, char *filename);
void printArray(int *arr, int size);
void runIterations(int i, int *lookupTable, const int lookupSize, int *cells, const int cellsSize, int saveToFile);
void writeToFile(char *fileName, int *history, int h, int w);

int main(int argc, char *argv[])
{
    char *rulename = "mod2.txt";
    char *initname = "k10.txt";
    int numOfIt = 100;
    if (argc == 4)
    {
        rulename = argv[1];
        initname = argv[2];
        numOfIt = atoi(argv[3]);
    }

    int lookuptable[8];
    int *cells;

    makeLookupTable(lookuptable, rulename);
    size_t cellsSize = getCellInfo(&cells, initname);
    runIterations(numOfIt, lookuptable, sizeof(lookuptable) / sizeof(lookuptable[0]), cells, cellsSize, 1);
    free(cells);
    return 0;
}

int f(int *lookupTable, int c1, int c2, int c3)
{
    if ((c1 != 1 && c1 != 0) || (c2 != 1 && c2 != 0) || (c3 != 1 && c3 != 0))
    {
        printf("weird values: %d, %d, %d\n", c1, c2, c3);
        return 0;
    }
    else
    {
        int index = c1 * 2 * 2 + c2 * 2 + c3;
        return lookupTable[index];
    }
}

char **str_split(char *a_str, const char a_delim)
{
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char *) * count);

    if (result)
    {
        size_t idx = 0;
        char *token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int makeLookupTable(int *lookupTable, char *filename)
{
    FILE *fp;
    char str[MAXCHAR];
    char *relpath = "../";
    char *path;
    path = calloc(1, strlen(relpath) + strlen(filename));
    strcat(path, relpath);
    strcat(path, filename);

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    int i = 0;
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        char **split = str_split(str, ' ');
        if (*(split + 1))
        {
            if (strcmp("1\n", *(split + 1)) == 0)
            {
                lookupTable[i] = 1;
            }
            else
            {
                lookupTable[i] = 0;
            }
            i++;
        }
    }
    fclose(fp);
    return 0;
}

int getCellInfo(int **cells, char *filename)
{
    FILE *fp;
    char str[MAXCHAR];
    char *p = "../";
    char *path = calloc(1, strlen(p) + strlen(filename) + 1);
    strcat(path, p);
    strcat(path, filename);
    int size = 0;
    int len = 0;

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    
    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        char *end;
        len = atoi(str);
    }
    
    int i = 0;
    char *binarystr = calloc(1, len+1);
    if (fgets(binarystr, len+1, fp) != NULL)
    {
        for (size_t j = 0; j < strlen(binarystr); j++)
        {
            if ('1' == binarystr[j] || '0' == binarystr[j])
                size++;
        }

        *cells = malloc(size * sizeof(int));

        for (size_t j = 0; j < strlen(binarystr); j++)
        {
            if ('1' == binarystr[j])
            {
                *(*(cells) + i) = 1;
                i++;
            }
            else if ('0' == binarystr[j])
            {
                *(*(cells) + i) = 0;
                i++;
            }
        }
    }
    fclose(fp);
    return size;
}

void printArray(int *arr, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

void runIterations(int numOfIt, int *lookupTable, const int lookupSize, int *cells, const int cellsSize, int saveToFile)
{
    if (cellsSize < 3)
    {
        return;
    }
    int *history = (int *)malloc(cellsSize * (numOfIt + 1) * sizeof(int));

    for (size_t i = 0; i < numOfIt; i++)
    {
        printf("t = %zu: ", i);
        printArray(cells, cellsSize);
        int firstVal = cells[0];
        int leftVal = cells[cellsSize - 1];
        int rightVal = cells[1];
        for (size_t j = 0; j < cellsSize; j++)
        {
            int middle = cells[j];
            int index = i * cellsSize + j;
            *(history + i * cellsSize + j) = middle;
            cells[j] = f(lookupTable, leftVal, middle, rightVal);
            leftVal = middle;
            if (j + 1 == cellsSize - 1)
            {
                rightVal = firstVal;
            }
            else
            {
                rightVal = cells[j + 2];
            }
        }
    }
    for (size_t k = 0; k < cellsSize; k++)
    {
        *(history + numOfIt * cellsSize + k) = cells[k];
    }

    printf("t = %d: ", numOfIt);
    printArray(cells, cellsSize);
    writeToFile("data.csv", history, cellsSize, numOfIt + 1);
}

void writeToFile(char *fileName, int *history, int w, int h)
{
    FILE *fp;
    fp = fopen("../Plot/data.csv", "w+");

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j + 1 < w; j++)
        {
            // printf("%d,", *(history + i*h + j));
            fprintf(fp, "%d,", *(history + i * w + j));
        }
        // printf("%d\n", *(history + i*h + w-1));
        fprintf(fp, "%d\n", *(history + i * w + w - 1));
    }
    fclose(fp);
}