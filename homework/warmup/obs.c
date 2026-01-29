
void print_result(const char *label, char **words, int n) {
    printf("%s:\n", label);
    printf("  num_words = %d\n", n);
    for (int i = 0; i < n; i++) {
        printf("  [%d] \"%s\"\n", i, words[i]);
    }
    printf("\n");
}

void free_words(char **words, int n) {
    for (int i = 0; i < n; i++) {
        free(words[i]);
    }
    free(words);
}

/*char **string_split(const char *input, const char *sep, int *num_words){
    *num_words = 0;
    char **words = (char **)malloc(sizeof(char *)*1);

    int currChar = 0;
    int blockSize = 0;
    while (input[currChar] != '\0'){
        blockSize = strspn(&input[currChar], sep);
        if (blockSize == 0){
            blockSize = strcspn(&input[currChar], sep);
            *num_words += 1;
            words = (char**)realloc(words, *num_words*sizeof(char*));

            words[*num_words-1] = (char*)malloc((blockSize+1)*sizeof(char));
            memcpy(
                words[*num_words], 
                &input[currChar], 
                blockSize*sizeof(char)
            );
            words[*num_words][blockSize] = '\0';
        }
        currChar += blockSize;
    }
    return words;
}*/
 /*
 char **string_split(const char *input, const char *sep, int *num_words){
    *num_words = 0;
    char **words = (char **)malloc(sizeof(char *)*1);

    char *currInputChar = &input[0];
    char *currSepChar = &sep[0];
    char *currWord = (char*)malloc(sizeof(char)*1);
    int wordLen = 1;
    while (*currInputChar != '\0'){
        if (*currInputChar == *currSepChar){
            currSepChar += 1;
            currInputChar += 1;
        } else if (*currSepChar == '\0'){
            *currSepChar = &sep[0];
            *num_words += 1;
            **words = realloc(**words, sizeof(char*)*(*num_words));
            currWord[wordLen-1] = '\0';
            words[*num_words-1] = currWord;

            wordLen = 1;
        } else { 
            wordLen += 1;
            *currWord = realloc(*currWord, sizeof(char)*(wordLen));
            currWord[wordLen-2] = *currInputChar; // skip over '\0'
            currInputChar += 1;
        }
    }
 };
 */