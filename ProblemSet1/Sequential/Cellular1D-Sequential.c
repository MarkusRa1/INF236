#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAXCHAR 1000

int f(int *lookupTable, int c1, int c2, int c3){
    int index = c1*2^2 + c2*2 + c3;
    return lookupTable[index];
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
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

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

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

int makeLookupTable(int *lookupTable){
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "..\\mod2.txt";
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    int i = 0;
    while (fgets(str, MAXCHAR, fp) != NULL) {
        char** split = str_split(str, ' ');
        if (*(split+1)) {
            if (strcmp("1\n", *(split+1)) == 0)
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

int getCellInfo(int *cells) {
        FILE *fp;
    char str[MAXCHAR];
    char* filename = "..\\middle30.txt";
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    int i = 0;
    while (fgets(str, 2, fp) != NULL) {
        if (strcmp("1", str) == 0) {
            cells[i] = 1;
            i++;
        } else if (strcmp("0", str) == 0) {
            cells[i] = 0;
            i++;
        }
    }
    fclose(fp);
    return 0;
}

void printArray(int *arr, int size) {
    for(size_t i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
    
}

void runIterations(int i, int *lookupTable, const int lookupSize, int *cells, const int cellsSize, int saveToFile) {
    if (cellsSize < 3) {
        return;
    }
    const int historySize = i;
    const int historyCellsSize = cellsSize;
    int history[60][3];
    int numOfIt = i;

    for(; i>0; i--) {
        printf("t = %d: ", numOfIt-i);
        printArray(cells, cellsSize);
        int firstVal = cells[0];
        int leftVal = cells[cellsSize-1];
        int rightVal = cells[0];
        for(size_t j = 0; j < cellsSize; j++)
        {
            int middle = cells[j];
            cells[j] = f(lookupTable, leftVal, middle, rightVal);
            leftVal = middle;
            if (j+1 == cellsSize-1) {
                rightVal = firstVal;
            } 
            else
            {
                rightVal = cells[j+2];
            }
        }
    }
    printf("t = %d:", numOfIt-i);
    printArray(cells, cellsSize);
}

void main()
{
    int bstr[6] = {1, 1, 1, 1, 1, 1};
    int lookuptable[8];
    int cells[60];

    makeLookupTable(lookuptable);
    getCellInfo(cells);
    runIterations(10, lookuptable, sizeof(lookuptable)/sizeof(lookuptable[0]), cells, sizeof(cells)/sizeof(cells[0]), 1);

    int val = f(lookuptable, 0,0,1);
    printArray(lookuptable, 8);
}