#include <mpi.h> // Header defining MPI functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAXCHAR 1000

int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(int **cells, char *filename);
char **str_split(char *a_str, const char a_delim);
void printArray(int *arr, int size);
void runIterations(int numOfIt, int *lookupTable, const int lookupSize, int *history, const int cellsSize, int my_rank, int comm_sz);
int f(int *lookupTable, int c1, int c2, int c3);
void writeToFile(char *fileName, int *history, int w, int h);
int mod(int x, int m);
void printMatrix(int *mat, int w, int h);

int main(int argc, char **argv)
{
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */

    MPI_Init(&argc, &argv); // No MPI function before this call
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int *lookuptable = malloc(8 * sizeof(int));
    int *cells;
    int *sendcount = malloc(comm_sz * sizeof(int));
    int cellsSize;
    char *rulename = "mod2.txt";
    char *initname = "k11.txt";
    int numOfIt = 100000;
    if (argc == 4)
    {
        rulename = argv[1];
        initname = argv[2];
        numOfIt = atoi(argv[3]);
    }

    if (my_rank == 0)
    {
        cellsSize = (int)getCellInfo(&cells, initname);

        int cellsPerProc = cellsSize / comm_sz;
        int rest = cellsSize - cellsPerProc * comm_sz;

        for (size_t i = 0; i < comm_sz; i++)
        {
            if (rest > 0)
            {
                sendcount[i] = cellsPerProc + 1;
                rest--;
            }
            else
            {
                sendcount[i] = cellsPerProc;
            }
        }
        makeLookupTable(lookuptable, rulename);
        // MPI_Scatter to split an array evenly
        // Assume that it breaks up evenly
        // Gather something something
        // Exercise 2 same thing?
    }

    MPI_Bcast(lookuptable, 8, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(sendcount, comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
    int sum = 0;
    int *displays = malloc(comm_sz * sizeof(int));
    for (size_t i = 0; i < comm_sz; i++)
    {
        displays[i] = sum;
        sum += sendcount[i];
    }

    int myCellsSize = sendcount[my_rank];
    int *recvarr = malloc(myCellsSize * sizeof(int));

    clock_t start, end1, end2;
    start = clock();
    MPI_Scatterv(cells, sendcount, displays, MPI_INT, &recvarr[0], myCellsSize, MPI_INT, 0, MPI_COMM_WORLD);

    int *history = (int *)malloc(myCellsSize * (numOfIt + 1) * sizeof(int));
    for (size_t i = 0; i < myCellsSize; i++)
    {
        *(history + i) = recvarr[i];
    }

    runIterations(numOfIt, lookuptable, 8, history, myCellsSize, my_rank, comm_sz);

    int *historyall;
    int *rbuf;
    if (my_rank == 0)
    {
        rbuf = (int *)malloc(cellsSize * sizeof(int));
        historyall = (int *)malloc(cellsSize * (numOfIt + 1) * sizeof(int));
    }
    end1 = clock();
    for (size_t i = 0; i < numOfIt + 1; i++)
    {
        MPI_Gatherv(history + i * myCellsSize, sendcount[my_rank], MPI_INT, historyall + i * cellsSize, sendcount, displays, MPI_INT, 0, MPI_COMM_WORLD);
    }
    end2 = clock();
    MPI_Finalize(); // No MPI function after this call
    if (my_rank == 0)
    {
        double t1 = ((double) (end1-start))/CLOCKS_PER_SEC;
        double t2 = ((double) (end2-start))/CLOCKS_PER_SEC;
        printf("%fs, %fs\n", t1, t2);
        // writeToFile("de", historyall, cellsSize, numOfIt + 1);
    }
    return 0;
}

int makeLookupTable(int *lookupTable, char *filename)
{
    FILE *fp;
    char str[MAXCHAR];
    char *relpath = "../";
    char *path;
    path = calloc(1, strlen(relpath) + strlen(filename) + 1);
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
                *(lookupTable + i) = 1;
            }
            else
            {
                *(lookupTable + i) = 0;
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

int mod(int x, int m)
{
    return (x % m + m) % m;
}

void runIterations(int numOfIt, int *lookupTable, int lookupSize, int *history, int cellsSize, int my_rank, int comm_sz)
{
    int rightneig = mod(my_rank + 1, comm_sz);
    int leftneig = mod(my_rank - 1, comm_sz);
    // printf("left right: %d %d %d\n", leftneig, rightneig, comm_sz);
    // printf("t = %d: ", 0);
    // printArray(history, cellsSize);
    int sz = (numOfIt + 1) * cellsSize;
    int leftVal;
    int rightVal;
    for (int i = 0; i < numOfIt; i++)
    {
        if (comm_sz > 1)
        {
            if (mod(my_rank, 2) == 0)
            {
                // printf("%d send to %d\n", my_rank, rightneig);
                MPI_Send(history + i * cellsSize + cellsSize - 1, 1, MPI_INT, rightneig, 1, MPI_COMM_WORLD);
                MPI_Recv(&rightVal, 1, MPI_INT, rightneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // printf("%d send to %d\n", my_rank, leftneig);
                MPI_Send(history + i * cellsSize, 1, MPI_INT, leftneig, 1, MPI_COMM_WORLD);
                MPI_Recv(&leftVal, 1, MPI_INT, leftneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                // printf("%d recv from %d\n", my_rank, leftneig);
                MPI_Recv(&leftVal, 1, MPI_INT, leftneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(history + i * cellsSize, 1, MPI_INT, leftneig, 1, MPI_COMM_WORLD);
                // printf("%d recv from %d\n", my_rank, rightneig);
                MPI_Recv(&rightVal, 1, MPI_INT, rightneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(history + i * cellsSize + cellsSize - 1, 1, MPI_INT, rightneig, 1, MPI_COMM_WORLD);
            }
            // printf("P%d done %d, got %d and %d\n", my_rank, i, leftVal, rightVal);
        }
        else
        {
            leftVal = *(history + i * cellsSize + cellsSize - 1);
            rightVal = *(history + i * cellsSize);
        }

        for (size_t j = 0; j < cellsSize; j++)
        {
            if (j == 0)
            {
                *(history + (i + 1) * cellsSize + j) = f(lookupTable, leftVal, *(history + i * cellsSize),
                                                         *(history + i * cellsSize + 1));
            }
            else if (j == cellsSize - 1)
            {
                *(history + (i + 1) * cellsSize + j) = f(lookupTable, *(history + i * cellsSize + j - 1),
                                                         *(history + i * cellsSize + j), rightVal);
                // printf("%d vs %d\n",(int) j, sz);
            }
            else
            {
                *(history + (i + 1) * cellsSize + j) = f(lookupTable, *(history + i * cellsSize + j - 1),
                                                         *(history + i * cellsSize + j), *(history + i * cellsSize + j + 1));
            }
        }
    }
    // printf("done\n");
}

int f(int *lookuptable, int c1, int c2, int c3)
{
    if ((c1 != 1 && c1 != 0) || (c2 != 1 && c2 != 0) || (c3 != 1 && c3 != 0))
    {
        // printf("weird values: %d, %d, %d\n", c1, c2, c3);
        return 0;
    }
    else
    {
        int index = c1 * 2 * 2 + c2 * 2 + c3;
        return *(lookuptable + index);
    }
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

void printArray(int *arr, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

void printMatrix(int *mat, int w, int h)
{
    for (int i = 0; i < h; i++)
    {
        printf("t = %3d: ", i);
        printArray(mat + i * w, w);
    }
}