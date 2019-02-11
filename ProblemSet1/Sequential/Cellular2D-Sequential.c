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
    runIterations(numOfIt, lookuptable, sizeof(lookuptable) / sizeof(lookuptable[0]), cells, w, h);
    // free(cells);
    return 0;
}

int f(int *lookupTable, int neigb[9])
{
    int weirdvals = 0;
    for (size_t i = 0; i < 9; i++)
    {
        if (neigb[i] != 0 && neigb[i] != 1)
        {
            weirdvals = 1;
        }
    }

    if (weirdvals)
    {
        printf("weird values:");
        printArray(neigb, 9);
        return 0;
    }
    else
    {
        int index = 0;
        int pow2 = 1;
        for (int i = 8; i >= 0; i--)
        {
            int n = neigb[i];
            index += n * pow2;
            pow2 *= 2;
        }

        // printf("%3d, %d <-", index, lookupTable[index]);
        // printArray(neigb, 9);
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
    if (path == NULL) {
        perror("Path failed alloc\n");
        exit(EXIT_FAILURE);
    }
    
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

    free(path);
    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        len = atoi(str);
    }
    *w = len;
    *h = len;

    *cells = malloc((*w) * (*h) * sizeof(int));
    if (*cells == NULL) {
        fclose(fp);
        perror("cells failed malloc\n");
        exit(EXIT_FAILURE);
    }
    
    char *binarystr = malloc((len + 2) * sizeof(char));
    if (binarystr == NULL) {
        free(cells);
        perror("binarystr failed malloc\n");
        exit(EXIT_FAILURE);
    }

    int iw = 0;
    for (int ih = 0; fgets(binarystr, len + 2, fp) != NULL; ih++)
    {
        if (binarystr[0] == '\n')
        {
            ih--;
        }
        else
        {
            iw = 0;
            for (size_t j = 0; j < *w; j++)
            {
                if ('1' == binarystr[j])
                {
                    *(*(cells) + *(w)*ih + iw) = 1;
                    iw++;
                }
                else if ('0' == binarystr[j])
                {
                    *(*(cells) + *(w)*ih + iw) = 0;
                    iw++;
                } else
                {
                    perror("Unexpected values in configuration file\n");
                }
                
            }
        }
    }
    free(binarystr);
    fclose(fp);
    return 0;
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
    int *history = malloc(totalhsz);
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            *(history + i * w + j) = *(cells + i * w + j);
        }
    }

    int neigb[9];
    for (size_t i = 0; i < numOfIt; i++)
    {
        int currit = mod(i, 2);
        int nextit = mod(i + 1, 2);
        for (size_t jh = 0; jh < h; jh++)
        {

            for (size_t jw = 0; jw < w; jw++)
            {
                int left = mod(jw - 1, w);
                int right = mod(jw + 1, w);
                int up = mod(jh - 1, h);
                int down = mod(jh + 1, h);
                int leftright[3] = {left, jw, right};
                int updown[3] = {up, jh, down};
                int ineig = 0;
                for (size_t col = 0; col < 3; col++)
                {
                    for (size_t row = 0; row < 3; row++)
                    {
                        neigb[ineig] = *(history + currit * w * h + updown[col] * w + leftright[row]);
                        ineig++;
                    }
                }
                int nextval = f(lookupTable, neigb);
                int k = nextit * w * h + jh * w + jw;
                *(history + nextit * w * h + jh * w + jw) = nextval;
            }
        }
    }
    // printMatrix(history + mod(numOfIt, 2) * w * h, w, h);
    writeToFile("data.csv", history + mod(numOfIt, 2) * w * h, w, h);
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
        printArray(mat + i * w, w);
    }
}

int mod(int x, int m)
{
    return (x % m + m) % m;
}