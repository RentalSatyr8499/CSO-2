#include "split.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *sep = (char*)malloc(2);
    if (argc == 1){
        sep[0] = '\t';
    } else {
        for (int i = 1; i < argc; i++){
            sep = realloc(sep, strlen(argv[i])*sizeof(char));
            strcat(sep, argv[i]);
        }
    }
    sep[strlen(sep)-1] = '\0';
    
    char input[5000];
    char **words;
    int n;
    while (1){
        scanf("%s", input);
        if (strcmp(input, ".") == 0){
            break;
        }

        words = string_split(input, sep, &n);

        printf("[");
        for (int i = 0; i < n; i++){
            printf("%s][", words[i]);
            free(words[i]);
        }
        printf("]\n");
        free(words);
    }
    return 0;
}
