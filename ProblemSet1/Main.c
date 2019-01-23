#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAXCHAR 1000

int f(int c1, int c2, int c3){
    
    
    switch (c1*100 + c2*10 + c3)
    {
        case 0:
            return 0;
        case 1:
            return 1;
        case 10:
            return 1;
        case 11:
            return 0;
        case 100:
            return 1;
        case 101:
            return 0;
        case 110:
            return 1;
        case 111:
            return 1;
        default:
            break;
    }
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
    char* filename = ".\\mod2.txt";
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    int i = 0;
    while (fgets(str, MAXCHAR, fp) != NULL) {
        printf("%s", str);
        char** split = str_split(str, ' ');

        if (*(split+1)) {
            if (strcmp("1", *(split+1)) == 0)
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

void main()
{
    int bstr[6] = {1, 1, 1, 0, 0, 1};
    int lookuptable[8];



    makeLookupTable(lookuptable);
    int val = f(lookuptable, 0,0,1);
    printf("%d", lookuptable[2]);
}