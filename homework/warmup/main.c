#include "split.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *sep;
    if (argc == 1){
        sep = (char*)malloc(3);
        sep[0] = ' ';
        sep[1] = '\t';
        sep[2] = '\0';
    } else {
        sep = malloc(1);
        sep[0] = '\0';
        for (int i = 1; i < argc; i++){
            sep = realloc(sep, strlen(sep) + strlen(argv[i])*sizeof(char) + 1);
            strcat(sep, argv[i]);
        }
    }
    
    char input[5000];
    char **words;
    int n;
    while (1){
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, ".") == 0){
            break;
        }

        words = string_split(input, sep, &n);

        printf("[");
        for (int i = 0; i < n-1; i++){
            printf("%s][", words[i]);
            free(words[i]);
        }
        printf("%s]\n", words[n-1]);
        free(words[n-1]);
        free(words);
    }
    free(sep);
    return 0;
}
