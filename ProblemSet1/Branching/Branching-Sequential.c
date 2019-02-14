#include <stdio.h>
#define MAXCHAR 255

char **str_split(char *a_str, const char a_delim);


int main(int argc, char *argv[])
{
    char *a = "0101";
    char *rulename = "branchingmod3.txt";
    if (argc == 2) {
        rulename = argv[0];
        *a = argv[1];
    }
    int *operation;
    int osz;
    int **ilookup = (int **)malloc(2*sizeof(int*));
    int asz;
    int *final;
    int fsz;
    int ssz;

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

void parseRuleFile(char *filename, int **operation, int *osz, int **ilookup, int *isz, int *ssz, int **final, int *fsz) {
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

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start");
        printf("start:%send\n", str);
        *osz = atoi(str);
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start");
        printf("start:%send\n", str);
        *isz = atoi(str);
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start");
        printf("start:%send\n", str);
        *ssz = atoi(str);
    }

    if (fgets(str, MAXCHAR, fp) != NULL) {
        printf("start");
        printf("start:%send\n", str);
        *fsz = atoi(str);
    }
    
    int o = 1, f = 0, il = 0;
    while(fgets(str, MAXCHAR, fp) != NULL){
        
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