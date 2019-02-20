/*
Copyright 2019 Markus Ragnhildstveit

Permission is hereby granted, free of charge, 
to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
int val(int vI1, int vI2, int *operation, int osz);
int valN(int *binarr, int bsz, int *operation, int osz, int *ilookup, int isz);
int lookup(int index, int val, int *ilookup, int isz);
int lookupBinary(int *binarr, int bsz, int *ilookup, int isz);
int isInF(int val, int *final, int fsz);
void generateRules(int msz, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz);
void generateAssignment(int *binArr, int bsz);
int mod(int x, int m);

int main(int argc, char *argv[])
{
    srand(1029);
    char *a = "0101";
    char *rulename = "branchingmod3.txt";
    if (argc == 3)
    {
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
    int bsz = 10;
    int *binarr = malloc(bsz * sizeof(int));

    generateRules(10, &operation, &osz, &ilookup, &isz, &ssz, &final, &fsz);
    generateAssignment(binarr, bsz);
    // parseRuleFile(rulename, &operation, &osz, &ilookup, &isz, &ssz, &final, &fsz);

    // stringToBinArr(a, binarr, bsz);

    printf("Binary arr: ");
    printArray(binarr, bsz);

    // printf("il mat: \n");
    // printMatrix(ilookup, 3, isz);
    // printf("0 + 2 mod 3 = %d\n", val(0, 2, operation, osz));
    // printf("lookup index 1: %d\n", lookup(1, 1, ilookup, isz));

    lookupBinary(binarr, bsz, ilookup, isz);
    printf("Arr: ");
    printArray(binarr, bsz);
    int result = valN(binarr, bsz, operation, osz, ilookup, isz);
    printf("Result val(P,a): %d\nIs in final: %d\n", result, isInF(result, final, fsz));

}

int isInF(int val, int *final, int fsz) 
{
    for(size_t i = 0; i < fsz; i++)
    {
        if (val == final[i]) {
            return 1;
        }
    }
    return 0;
}

int valN(int *binarr, int bsz, int *operation, int osz, int *ilookup, int isz) 
{
    for(int i = bsz-2; i >= 0; i--)
    {
        int v1 = binarr[i];
        int v2 = binarr[i+1];
        binarr[i] = val(v1, v2, operation, osz);
        int k = 0;
    }
    return binarr[0];
}

int lookup(int index, int val, int *ilookup, int isz)
{
    for(size_t i = 0; i < isz; i++)
    {
        if (*(ilookup + i * 3) == index) {
            if (val) {
                return *(ilookup + i * 3 + 2);
            } else
            {
                return *(ilookup + i * 3 + 1);
            }
        }
        
    }
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

int lookupBinary(int *binarr, int bsz, int *ilookup, int isz) 
{
    for(size_t index = 0; index < bsz; index++)
    {
        binarr[index] = lookup(index, binarr[index], ilookup, isz);
    }
}

int val(int vI1, int vI2, int *operation, int osz)
{
    for (size_t i = 0; i < osz; i++)
    {
        if (*(operation + i * 3) == vI1 && *(operation + i * 3 + 1) == vI2)
        {
            return *(operation + i * 3 + 2);
        }
    }
    perror("Combination not found");
    exit(-1);
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