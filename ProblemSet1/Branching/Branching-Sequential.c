#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAXCHAR 255

char **str_split(char *a_str, const char a_delim, int *sz);
int parseRuleFile(char *filename, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz);
void stringToBinArr(char *binstr, int *arr, int n);
void printMatrix(int *mat, int w, int h);
void printArray(int *arr, int size);

int main(int argc, char *argv[])
{
    char *a = "0101";
    char *rulename = "branchingmod3.txt";
    if (argc == 3) {
        rulename = argv[1];
        a = argv[2];
    }
    int *operation;
    int osz;
    int *ilookup;
    int isz;
    int asz;
    int *final;
    int fsz;
    int ssz;
    parseRuleFile(rulename, &operation, &osz, &ilookup, &isz, &ssz, &final, &fsz);
    printMatrix(ilookup, isz, 3);
}

void stringToBinArr(char *binstr, int *arr, int n) {
    for(size_t i = 0; i < n; i++)
    {
        if (binstr[i] == '0') {
            arr[i] = 0;
        } else if (binstr[i] == '1')
        {
            arr[i] = 1;
        } else
        {
            perror("Weird value in a");
        }
    }
}

int parseRuleFile(char *filename, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz) {
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

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start:%send\n", str);
        *osz = atoi(str);
        printf("start:%dend\n", *osz);
        *operation = malloc(*osz * 3 * sizeof(int));
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start:%send\n", str);
        *isz = atoi(str);
        printf("start:%dend\n", *isz);
        *ilookup = malloc(*isz * 3 * sizeof(int));
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start:%send\n", str);
        *ssz = atoi(str);
        printf("start:%dend\n", *ssz);
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start:%send\n", str);
        *fsz = atoi(str);
        printf("start:%dend\n", *fsz);
        *final = malloc(*fsz * sizeof(int));
    }
    
    int o = 1, f = 0, l = 0;
    char **splt;
    int spltsz;
    int iop = 0;
    int ifinal = 0;
    int iilook = 0;
    while(fgets(str, MAXCHAR, fp) != NULL){
        printf("%s", str);
        splt = str_split(str, ' ', &spltsz);
        if (o) {
            if (spltsz == 1) {
                o = 0;
                f = 1;
            } else
            {
                *(*(operation) + iop) = atoi(splt[0]);
                *(*(operation) + iop + 1) = atoi(splt[1]);
                *(*(operation) + iop + 2) = atoi(splt[2]);
                iop += 3;
            }
        }
        if (f) {
            if (spltsz == 3) {
                f = 0;
                l = 1;
            } else
            {
                *(*(final) + ifinal) = atoi(splt[0]);
                ifinal++;
            }            
        }
        if (l && spltsz == 3) {
            *(*(ilookup) + iilook) = atoi(splt[0]);
            *(*(ilookup) + *isz + iilook) = atoi(splt[1]);
            *(*(ilookup) + 2 * *isz + iilook) = atoi(splt[2]);
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