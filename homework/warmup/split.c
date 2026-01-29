#include <string.h>
#include <stdlib.h>

#include <stdio.h>

char **string_split(const char *input, const char *sep, int *num_words){
    *num_words = 0;
    char **words = (char **)malloc(sizeof(char *)*1);

    printf("separator: %s\n", sep);

    int baseIndex = 0, separatorLen = 0, wordLen = 0;
    while (1){
        if (input[baseIndex] == '\0'){
            break;
        }
        wordLen = strcspn(&input[baseIndex], sep);
        separatorLen = strspn(&input[baseIndex+wordLen], sep);

        *num_words += 1;
        words = (char**)realloc(words, *num_words*sizeof(char*));

        words[*num_words-1] = (char*)malloc((wordLen+1)*sizeof(char));
        memcpy(
            words[*num_words-1], 
            &input[baseIndex], 
            wordLen*sizeof(char)
        );
        words[*num_words-1][wordLen] = '\0';
        baseIndex += separatorLen + wordLen;
    }

    printf("found null byte at %d, returning words", baseIndex);

    return words;
}




