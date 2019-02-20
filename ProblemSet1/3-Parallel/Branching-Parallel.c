#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <mpi.h>
#define MAXCHAR 255

char **str_split(char *a_str, const char a_delim, int *sz);
int parseRuleFile(char *filename, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz);
void stringToBinArr(char *binstr, int *arr, int n);
void printMatrix(int *mat, int w, int h);
void printArray(int *arr, int size);
int val(int vI1, int vI2, int *operation, int osz);
int valN(int *binarr, int bsz, int *operation, int osz);
int lookup(int index, int val, int *ilookup, int isz, int display);
int lookupBinary(int *binarr, int bsz, int *ilookup, int isz, int display);
int isInF(int val, int *final, int fsz);
void valmpi(int *in, int *inout, int *len, MPI_Datatype *dptr);
int mod(int x, int m);
void generateAssignment(int *binArr, int bsz);
void generateRules(int msz, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz);

int main(int argc, char **argv)
{
    int comm_sz;            /* Number of processes */
    int my_rank;            /* My process rank */
    MPI_Init(&argc, &argv); // No MPI function before this call
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Op op;
    char *a = "0101";
    char *rulename = "branchingmod3.txt";
    int bsz;
    if (argc == 3)
    {
        rulename = argv[1];
        a = argv[2];
        // bsz = atoi(argv[2]);
    }
    bsz = sizeof(a);
    int *operation;
    int osz;
    int *ilookup;
    int isz;
    int asz;
    int *final;
    int fsz;
    int ssz;
    int *binarr = malloc(bsz * sizeof(int));
    int *sendcount = malloc(comm_sz * sizeof(int));
    int *myBinArr;
    int mybsz;

    if (my_rank == 0)
    {
        parseRuleFile(rulename, &operation, &osz, &ilookup, &isz, &ssz, &final, &fsz);
        stringToBinArr(a, binarr, bsz);
        // generateRules(1000, &operation, &osz, &ilookup, &isz, &ssz, &final, &fsz);
        // generateAssignment(binarr, bsz);

        int perProc = bsz / comm_sz;
        int rest = bsz - perProc * comm_sz;

        for (size_t i = 0; i < comm_sz; i++)
        {
            if (rest > 0)
            {
                sendcount[i] = perProc + 1;
                rest--;
            }
            else
            {
                sendcount[i] = perProc;
            }
        }
        // printMatrix(operation, 3, osz);
    }

    MPI_Bcast(&osz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&isz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&asz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&fsz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&ssz, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0)
    {
        operation = malloc(osz * 3 * sizeof(int));
        ilookup = malloc(isz * 3 * sizeof(int));
        final = malloc(fsz * sizeof(int));
    }

    MPI_Bcast(operation, osz * 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(ilookup, isz * 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(final, fsz, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(sendcount, comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
    int sum = 0;
    int *displays = malloc(comm_sz * sizeof(int));
    for (size_t i = 0; i < comm_sz; i++)
    {
        displays[i] = sum;
        sum += sendcount[i];
    }

    mybsz = sendcount[my_rank];
    myBinArr = malloc(mybsz * sizeof(int));
    MPI_Scatterv(binarr, sendcount, displays, MPI_INT, myBinArr, mybsz, MPI_INT, 0, MPI_COMM_WORLD);
    free(binarr);
    lookupBinary(myBinArr, mybsz, ilookup, isz, displays[my_rank]);
    int result = valN(myBinArr, mybsz, operation, osz);
    
    int *resandop = malloc((1 + osz * 3) * sizeof(int));
    *resandop = result;
    for(size_t i = 0; i < osz * 3; i++)
    {
        *(resandop + i + 1) = operation[i];
    }
    free(operation);
    
    int *recvbf = malloc((1 + osz * 3) * sizeof(int));
    MPI_Op_create((MPI_User_function *) valmpi, 1, &op);

    MPI_Reduce(resandop, recvbf, 1 + osz * 3, MPI_INT, op, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        // printf("%d\n", *recvbf);
        // printf("1 + 0 = %d\n", val(1, 0, operation, osz));
        printf("Is in final: %d\n", isInF(*recvbf, final, fsz));
    }
    
    MPI_Finalize(); // No MPI function after this call
    return 0;
}

void valmpi(int *in, int *inout, int *len, MPI_Datatype *dptr)
{
    // printf("%d, %d\n", *in, *inout);
    
    *inout = val(*in, *inout, inout+1, (*len - 1)/3);
}

int isInF(int val, int *final, int fsz)
{
    for (size_t i = 0; i < fsz; i++)
    {
        if (val == final[i])
        {
            return 1;
        }
    }
    return 0;
}

int valN(int *binarr, int bsz, int *operation, int osz)
{
    for (int i = bsz - 2; i >= 0; i--)
    {
        int v1 = binarr[i];
        int v2 = binarr[i + 1];
        binarr[i] = val(v1, v2, operation, osz);
        // printf("i = %d, binarr[i] = %d, val = %d, %d, %d\n", i, binarr[i], val(v1, v2, operation, osz), v1, v2);
    }
    // printf("%d\n", *binarr);
    return *binarr;
}

int lookup(int index, int val, int *ilookup, int isz, int display)
{
    for(size_t i = 0; i < isz; i++)
    {
        if (*(ilookup + i * 3) == display + index) {
            if (val) {
                return *(ilookup + i * 3 + 2);
            } else
            {
                return *(ilookup + i * 3 + 1);
            }
        }
        
    }
}

int lookupBinary(int *binarr, int bsz, int *ilookup, int isz, int display) 
{
    for(size_t index = 0; index < bsz; index++)
    {
        binarr[index] = lookup(index, binarr[index], ilookup, isz, display);
    }
}

int val(int vI1, int vI2, int *operation, int osz)
{
    for (size_t i = 0; i < osz; i++)
    {
        
        if (*(operation + i * 3) == vI1 && *(operation + i * 3 + 1) == vI2)
        {
            // printf("%d, %d, %d\n", vI1, vI2, *(operation + i * 3 + 2));
            return *(operation + i * 3 + 2);
        }
    }
    printf("Combination not found: %d, %d\n", vI1, vI2);
    return -1;
}

void stringToBinArr(char *binstr, int *arr, int n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (binstr[i] == '0')
        {
            arr[i] = 0;
        }
        else if (binstr[i] == '1')
        {
            arr[i] = 1;
        }
        else
        {
            perror("Weird value in a");
        }
    }
}

int parseRuleFile(char *filename, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz)
{
    FILE *fp;
    char str[MAXCHAR];
    char *relpath = "../";
    char *path;
    path = calloc(1, strlen(relpath) + strlen(filename));
    strcat(path, relpath);
    strcat(path, filename);

    fp = fopen(path, "r");
    // free(path);
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }

    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        // printf("start:%send\n", str);
    }

    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        // printf("start:%send\n", str);
        *isz = atoi(str);
        // printf("start:%dend\n", *isz);
        *ilookup = malloc(*isz * 3 * sizeof(int));
    }

    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        // printf("start:%send\n", str);
        *ssz = atoi(str);
        // printf("start:%dend\n", *ssz);
        *osz = *ssz * *ssz;
        // printf("start:%dend\n", *osz);
        *operation = malloc(*osz * 3 * sizeof(int));
    }

    if (fgets(str, MAXCHAR, fp) != NULL)
    {
        // printf("start:%send\n", str);
        *fsz = atoi(str);
        // printf("start:%dend\n", *fsz);
        *final = malloc(*fsz * sizeof(int));
    }

    int o = 1, f = 0, l = 0;
    char **splt;
    int spltsz;
    int iop = 0;
    int ifinal = 0;
    int iilook = 0;
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        // printf("%s", str);
        splt = str_split(str, ' ', &spltsz);
        if (o)
        {
            if (spltsz == 2)
            {
                o = 0;
                f = 1;
            }
            else
            {
                *(*(operation) + iop) = atoi(splt[0]);
                *(*(operation) + iop + 1) = atoi(splt[1]);
                *(*(operation) + iop + 2) = atoi(splt[2]);
                iop += 3;
            }
        }
        if (f)
        {
            if (spltsz == 4)
            {
                f = 0;
                l = 1;
            }
            else
            {
                *(*(final) + ifinal) = atoi(splt[0]);
                ifinal++;
            }
        }
        if (l && (spltsz == 4 || spltsz == 3))
        {
            *(*(ilookup) + iilook * 3) = atoi(splt[0]);
            *(*(ilookup) + iilook * 3 + 1) = atoi(splt[1]);
            *(*(ilookup) + iilook * 3 + 2) = atoi(splt[2]);
            iilook++;
        }
    }
    return 0;
}

char **str_split(char *a_str, const char a_delim, int *sz)
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
    *sz = count;

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

void generateRules(int msz, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz) 
{
    *isz = msz;
    *ilookup = malloc(*isz * 3 * sizeof(int));
    *ssz = 60;
    *fsz = mod(rand(), *ssz);
    *final = malloc(*fsz * sizeof(int));
    for(size_t i = 0; i < *fsz; i++)
    {
        (*final)[i] = mod(rand(), *ssz);
    }
    *osz = *ssz * *ssz;
    *operation = malloc(*osz * 3 * sizeof(int));
    *isz = msz;
    *ilookup = malloc(*isz * 3 * sizeof(int));

    int iop = 0;
    for(size_t i = 0; i < *ssz; i++)
    {
        for(size_t j = 0; j < *ssz; j++)
        {
            (*operation)[iop * 3] = i;
            (*operation)[iop * 3 + 1] = j;
            (*operation)[iop * 3 + 2] = mod(i*j, *ssz);
            iop++;
        }
    }

    for(size_t i = 0; i < *isz; i++)
    {
        (*ilookup)[i * 3] = i;
        (*ilookup)[i * 3 + 1] = mod(rand(), *ssz);
        (*ilookup)[i * 3 + 2] = mod(rand(), *ssz);
    }
}

void generateAssignment(int *binArr, int bsz) 
{
    for(size_t i = 0; i < bsz; i++)
    {
        binArr[i] = mod(rand(), 2);
    }
    
}

int mod(int x, int m)
{
    return (x % m + m) % m;
}