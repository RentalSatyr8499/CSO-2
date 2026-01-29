int my_system(const char *command){
    *nCommands = 0;
    **commands = splitby(command, ";", nCommands);
    for (int i = 0; i < nCommands; i++){
        **args = splitby(command[i], " "); 
        add '\0' to args
        execve(args[0], args);
    }
    return 
}



splitby(string, delimiter, *n){
    int i = 0;
    char **splitStr;
    *n = 0
    while (string[i] != '\0'){
        nextStr = strcspn(string[i], delimiter);
        malloc space for nextStr
        splitStr.append(nextStr)
        *n += 1
    }
    return splitStr
}