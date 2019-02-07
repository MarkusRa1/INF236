#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#define MAXCHAR 1000

int f(int *lookupTable, int neigb[8]);
char **str_split(char *a_str, const char a_delim);
int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(int **cells, char *filename, int *w, int *h);
void printArray(int *arr, int size);
void runIterations(int i, int *lookupTable, const int lookupSize, int *cells, const int w, const int h);
void writeToFile(char *fileName, int *history, int h, int w);
void printMatrix(int *mat, int w, int h);
int mod(int x, int m);

int main(int argc, char *argv[])
{
    char *rulename = "gameOfLife.txt";
    char *initname = "config2D_20.txt";
    int numOfIt = 1;
    if (argc == 4)
    {
        rulename = argv[1];
        initname = argv[2];
        numOfIt = atoi(argv[3]);
    }

    int lookuptable[512];
    int *cells;
    int w, h;

    makeLookupTable(lookuptable, rulename);
    getCellInfo(&cells, initname, &w, &h);
    printf("test1\n");
    runIterations(numOfIt, lookuptable, sizeof(lookuptable) / sizeof(lookuptable[0]), cells, w, h);
    // free(cells);
    return 0;
}

int f(int *lookupTable, int neigb[8])
{
    int weirdvals = 0;
    for(size_t i = 0; i < 8; i++)
    {
        if (neigb[i] != 0 && neigb[i] != 0) {
            weirdvals = 1;
        }
    }
    
    if (weirdvals)
    {
        printf("weird values:");
        printArray(neigb, 8);
        return 0;
    }
    else
    {
        int index = 0;
        int pow2 = 1;
        for(size_t i = 0; i < 8; i++)
        {
            index += neigb[i] * pow2;
            pow2 *= 2;
        }
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

int getCellInfo(int **cells, char *filename, int *w, int *h)
{
    FILE *fp;
    char str[MAXCHAR];
    char *p = "../";
    char *path = calloc(strlen(p) + strlen(filename) + 1, sizeof(char));
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
    *w = len;
    *h = len;
    *cells = malloc(len * len * sizeof(int)); 
    // printf("test2\n");
    int ih = 0;
    int iw = 0;
    char *binarystr = calloc(len+1, sizeof(char));
    while (fgets(binarystr, len+1, fp) != NULL)
    {
        iw = 0;
        for (size_t j = 0; j < strlen(binarystr); j++)
        {
            if ('1' == binarystr[j] || '0' == binarystr[j])
                size++;
        }

        for (size_t j = 0; j < strlen(binarystr); j++)
        {
            if ('1' == binarystr[j])
            {
                *(*(cells) + len*ih + iw) = 1;
                iw++;
            }
            else if ('0' == binarystr[j])
            {
                *(*(cells) + len*ih + iw) = 0;
                iw++;
            }
        }
        ih++;
    }
    printf("test3\n");
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

void runIterations(int numOfIt, int *lookupTable, const int lookupSize, int *cells, int w, int h)
{
    int totalhsz = w * h * 2 * sizeof(int);
    printf("hello");
    int *history = malloc(totalhsz);
    for(size_t i = 0; i < h; i++)
    {
        for(size_t j = 0; j < w; j++)
        {
            *(history + i*w + j) = *(cells + i*w + j);
        }
    }
    printMatrix(history, w, h);

    int neigb[9];
    for (size_t i = 0; i < numOfIt; i++)
    {
        int currit = mod(i, 2);
        int nextit = mod(i+1, 2);
        for (size_t jh = 0; jh < h; jh++)
        {
            for(size_t jw = 0; jw < w; jw++)
            {
                int left = mod(jw - 1, w);
                int right = mod(jw + 1, w);
                int up = mod(jh - 1, h);
                int down = mod(jh + 1, h);
                int leftright[3] = {left, jw, right};
                int updown[3] = {up, jh, down};
                int ineig = 0;
                for(size_t row = 0; row < 3; row++)
                {
                    for(size_t col = 0; col < 3; col++)
                    {
                        neigb[ineig] = *(history + currit*w*h + updown[col]*w + leftright[row]);
                    }
                }
                int nextval = f(lookupTable, neigb);
                *(history + nextit*w*h + jh*w + jw) = nextval;
            }
        }
    }

    writeToFile("data.csv", history + mod(numOfIt, 2), w, h);
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

void printMatrix(int *mat, int w, int h)
{
    for (int i = 0; i < h; i++)
    {
        printf("t = %3d: ", i);
        printArray(mat + i * w, w);
    }
}

int mod(int x, int m)
{
    return (x % m + m) % m;
}