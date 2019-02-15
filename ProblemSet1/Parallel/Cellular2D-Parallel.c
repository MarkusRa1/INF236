#include <mpi.h> // Header defining MPI functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAXCHAR 1000

int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(short **cells, char *filename, int *w, int *h);
char **str_split(char *a_str, const char a_delim);
void printArray(int *arr, int size);
void runIterations(int numOfIt, int *lookupTable, const int lookupSize, short *history, int w, int h, int my_rank, int comm_sz);
int f(int *lookupTable, int neigb[9]);
void writeToFile(char *fileName, int *history, int w, int h);
int mod(int x, int m);
void printMatrix(int *mat, int w, int h);
void generateConfig(short *cnfg, int sz);

int main(int argc, char **argv)
{
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */

    MPI_Init(&argc, &argv); // No MPI function before this call
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int *lookuptable = malloc(512 * sizeof(int));
    short *cells;
    int w, h;
    int *sendcount = malloc(comm_sz * sizeof(int));
    char *rulename = "gameOfLife.txt";
    char *initname = "config2D_5.txt";
    int numOfIt = 10000;
    int sz = 10;
    if (argc == 4)
    {
        rulename = argv[1];
        sz = atoi(argv[2]);
        numOfIt = atoi(argv[3]);
    }

    if (my_rank == 0)
    {
        // getCellInfo(&cells, initname, &w, &h);
        w = sz;
        h = sz;
        cells = malloc(w * h * sizeof(short));
        generateConfig(cells, sz);

        int rowsPerProc = h / comm_sz;
        int rest = h - rowsPerProc * comm_sz;

        for (size_t i = 0; i < comm_sz; i++)
        {
            if (rest > 0)
            {
                sendcount[i] = (rowsPerProc + 1) * w;
                rest--;
            }
            else
            {
                sendcount[i] = rowsPerProc * w;
            }
        }
        makeLookupTable(lookuptable, rulename);
    }

    MPI_Bcast(lookuptable, 512, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(sendcount, comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int sum = 0;
    int *displays = malloc(comm_sz * sizeof(int));
    for (size_t i = 0; i < comm_sz; i++)
    {
        displays[i] = sum;
        sum += sendcount[i];
    }

    int myCellsSize = sendcount[my_rank];

    short *history = malloc(myCellsSize * 2 * sizeof(short));
    int myH = myCellsSize / w;

    double start, end1, end2;
    start = MPI_Wtime();

    MPI_Scatterv(cells, sendcount, displays, MPI_SHORT, history, myCellsSize, MPI_SHORT, 0, MPI_COMM_WORLD);
    if (my_rank == 0)
    {
        free(cells);
    }

    runIterations(numOfIt, lookuptable, 512, history, w, myH, my_rank, comm_sz);
    end1 = MPI_Wtime();
    free(lookuptable);

    short *historyall;
    if (my_rank == 0)
    {
        historyall = (short *)malloc(w * h * sizeof(short));
    }

    MPI_Gatherv(history + mod(numOfIt, 2) * w * myH, sendcount[my_rank], MPI_SHORT, historyall, sendcount, displays, MPI_SHORT, 0, MPI_COMM_WORLD);

    end2 = MPI_Wtime();
    free(history);
    free(sendcount);
    MPI_Finalize(); // No MPI function after this call
    if (my_rank == 0)
    {
        double t1 = (end1 - start);
        double t2 = (end2 - start);
        printf("%lu bits/bytes\n", sizeof(short));
        printf("size: %d\niterations: %d\ntime taken: %fs, %fs\nSpeed: %fMBps\n", sz, numOfIt, t1, t2, ((double)sz * sz * numOfIt) / t2 * sizeof(short) / 1000000);
        // printMatrix(historyall, w, h);
        // writeToFile("de", historyall, w, h);
        free(historyall);
    }
    return 0;
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
    free(path);
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
            if (*(split + 1)[0] == '1')
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

int getCellInfo(short **cells, char *filename, int *w, int *h)
{
    FILE *fp;
    char str[MAXCHAR];
    char *p = "../";
    char *path = calloc(strlen(p) + strlen(filename) + 1, sizeof(char));
    if (path == NULL)
    {
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
    if (*cells == NULL)
    {
        fclose(fp);
        perror("cells failed malloc\n");
        exit(EXIT_FAILURE);
    }

    char *binarystr = malloc((len + 2) * sizeof(char));
    if (binarystr == NULL)
    {
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
                }
                else
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

int mod(int x, int m)
{
    return (x % m + m) % m;
}

void runIterations(int numOfIt, int *lookupTable, int lookupSize, short *history, int w, int h, int my_rank, int comm_sz)
{
    int downneig = mod(my_rank + 1, comm_sz);
    int upneig = mod(my_rank - 1, comm_sz);
    short *updownarr = malloc(w * 2 * sizeof(short));
    int neigb[9];
    int even = (mod(my_rank, 2) == 0);
    int oddcmsz = (mod(comm_sz, 2) == 1);

    for (int i = 0; i < numOfIt; i++)
    {
        int currit = mod(i, 2);
        int nextit = mod(i + 1, 2);
        if (comm_sz > 1)
        {
            if (even)
            {
                // printf("%d send to %d\n", my_rank, downneig);
                MPI_Send(history + currit * w * h + w * (h - 1), w, MPI_SHORT, downneig, 1, MPI_COMM_WORLD);
                MPI_Recv(updownarr + w, w, MPI_SHORT, downneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // printf("%d send to %d\n", my_rank, upneig);
                if (my_rank == 0 && oddcmsz)
                {
                    MPI_Recv(updownarr, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(history + currit * w * h, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD);
                }
                else
                {
                    MPI_Send(history + currit * w * h, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD);
                    MPI_Recv(updownarr, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }

                // printf("%d: ", my_rank);
                // printArray(updownarr, w*2);
            }
            else
            {
                // printf("%d recv from %d\n", my_rank, upneig);
                MPI_Recv(updownarr, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(history + currit * w * h, w, MPI_SHORT, upneig, 1, MPI_COMM_WORLD);
                // printf("%d recv from %d\n", my_rank, downneig);
                MPI_Recv(updownarr + w, w, MPI_SHORT, downneig, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(history + currit * w * h + w * (h - 1), w, MPI_SHORT, downneig, 1, MPI_COMM_WORLD);
                // printf("%d: ", my_rank);
                // printArray(updownarr, w*2);
            }
            // printf("P%d done %d, got %d and %d\n", my_rank, i, leftVal, rightVal);
        }
        else
        {
            // leftVal = *(history + i * cellsSize + cellsSize - 1);
            // rightVal = *(history + i * cellsSize);
            for (size_t jw = 0; jw < w; jw++)
            {
                *(updownarr + jw) = *(history + currit * w * h + (h - 1) * w + jw);
                *(updownarr + w + jw) = *(history + currit * w * h + jw);
            }
        }

        for (size_t jh = 0; jh < h; jh++)
        {
            for (size_t jw = 0; jw < w; jw++)
            {
                short leftright[3] = {mod(jw - 1, w), jw, mod(jw + 1, w)};
                short updown[3] = {mod(jh - 1, h), jh, mod(jh + 1, h)};
                short ineig = 0;
                for (size_t col = 0; col < 3; col++)
                {
                    for (size_t row = 0; row < 3; row++)
                    {
                        if (jh == 0 && col == 0)
                        {
                            neigb[ineig] = *(updownarr + leftright[row]);
                        }
                        else if (jh == h - 1 && col == 2)
                        {
                            neigb[ineig] = *(updownarr + w + leftright[row]);
                        }
                        else
                        {
                            neigb[ineig] = *(history + currit * w * h + updown[col] * w + leftright[row]);
                        }
                        ineig++;
                    }
                }
                short nextval = f(lookupTable, neigb);
                *(history + nextit * w * h + jh * w + jw) = (short)nextval;
            }
        }
    }
    // printMatrix(history + mod(numOfIt, 2) * w * h, w, h);
    free(updownarr);
}

int f(int *lookupTable, int neigb[9])
{
    for (size_t i = 0; i < 9; i++)
    {
        if (neigb[i] != 0 && neigb[i] != 1)
        {
            printf("weird values:");
            printArray(neigb, 9);
            return 0;
        }
    }
    int index = 0;
    int pow2 = 1;
    for (int i = 8; i >= 0; i--)
    {
        int n = neigb[i];
        index += n * pow2;
        pow2 *= 2;
    }
    return lookupTable[index];
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

void writeToFile(char *fileName, int *history, int w, int h)
{
    FILE *fp;
    fp = fopen("../Plot/data.csv", "w+");

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j + 1 < w; j++)
        {
            fprintf(fp, "%d,", *(history + i * w + j));
        }
        fprintf(fp, "%d\n", *(history + i * w + w - 1));
    }
    fclose(fp);
}

void generateConfig(short *cnfg, int sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        for (size_t j = 0; j < sz; j++)
        {
            *(cnfg + sz * i + j) = (short)mod(rand(), 2);
        }
    }
}