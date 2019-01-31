#include <mpi.h> // Header defining MPI functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#define MAXCHAR 1000

int makeLookupTable(int *lookupTable, char *filename);
int getCellInfo(int **cells, char *filename);
char **str_split(char *a_str, const char a_delim);

int main(int argc, char **argv)
{
    int comm_sz;            /* Number of processes */
    int my_rank;            /* My process rank */
    MPI_Init(&argc, &argv); // No MPI function before this call
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int lookuptable[8];
    int neighbourcells[3];
    int *cells;
    if (my_rank != 0)
    {
        int *test;
        int what = 11*sizeof(int);
        MPI_Recv(test, what, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("rank %d: %d, %d\n", my_rank, test[0], test[1]);
        
    }
    else
    {
        printf("rank 0\n");
        char *rulename = "mod2.txt";
        char *initname = "middle30.txt";
        int numOfIt = 100;
        if (argc == 4)
        {
            rulename = argv[1];
            initname = argv[2];
            numOfIt = atoi(argv[3]);
        }
        makeLookupTable(lookuptable, rulename);
        size_t cellsSize = getCellInfo(&cells, initname);
        printf("read files done\n");
        //cellsPerProc = cellsSize / comm_sz;
        int msg[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        for(int i = 1; i < comm_sz; i++)
        {
            MPI_Send(&msg, 10*sizeof(int), MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        // MPI_Scatter to split an array evenly
        // Assume that it breaks up evenly
        // Gather something something
        // Exercise 2 same thing?
        
    }
    // Code
    MPI_Finalize(); // No MPI function after this call
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
    char *path;
    path = calloc(1, strlen("../") + strlen(filename));
    strcat(path, "../");
    strcat(path, filename);
    size_t size = 0;

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    int i = 0;
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        for (size_t j = 0; j < strlen(str); j++)
        {
            if ('1' == str[j] || '0' == str[j])
                size++;
        }

        *cells = malloc(strlen(str) * sizeof(cells[0]));
        for (size_t j = 0; j < strlen(str); j++)
        {
            if ('1' == str[j])
            {
                (*cells)[i] = 1;
                i++;
            }
            else if ('0' == str[j])
            {
                (*cells)[i] = 0;
                i++;
            }
        }
    }
    fclose(fp);
    return size;
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
