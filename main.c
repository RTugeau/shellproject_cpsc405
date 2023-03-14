#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_PARAMS 10

//comment so that I can make a test push

enum cmds {CD=0, EXIT, CAT, DOG, LS, PWD};

char *cmdstr[] = {"cd","exit","cat","dog", "ls", "pwd"};

int main()
{   
    char cmd[MAX_COMMAND_LENGTH + 1];
    char* params[MAX_PARAMS+1];
    int cmdCount=0,nparams=0;

    while(1){
        nparams=0;
        char*username = getenv("USER");
        printf("%s@snailShell %d> " , username, ++cmdCount);
        if(fgets(cmd,sizeof(cmd),stdin) == NULL) break;
        if(cmd[strlen(cmd)-1] == '\n') cmd[strlen(cmd)-1] = '\0';
        parseCmd(cmd, params, &nparams);
        if(strcmp(params[0], "exit") == 0) break;
        if(executeCmd(params, nparams) == 0) break;
    }

    return 0;
}



// Split cmd into array of parameters
void parseCmd(char* cmd, char** params, int *nparams)
{
    for(int i = 0; i < MAX_PARAMS; i++) {
        params[i] = strsep(&cmd, " ");
        if(params[i] == NULL) break;
        (*nparams)++;
    }   
}

int executeCmd(char** params, int nparams)
{
    int rc = 1;
    int ncmds = sizeof(cmdstr) / sizeof(char *); 
    int cmd_index;
    for (cmd_index = 0; cmd_index < ncmds; cmd_index++)
        if (strcmp(params[0], cmdstr[cmd_index]) == 0)
            break;

    switch (cmd_index){
        case CD: 
            if(nparams == 2){ 
                if(cd(params[1]) == 0){ 
                    //if cd() returns 0, that means everything is ok :)
                }
            }
            else{printf("cd command broke, that's your fault loser.\n");}
            break;

        case EXIT:
            rc=0;
            break;

        case CAT:
            if( nparams == 2 ){
                if( catHandler(params[1]) == 0){ 
                    //all good.
                }
            }
            else{printf("cat command broke. make sure you are using only one param.\n");}
            break;

        case DOG:
            if( nparams == 2){ 
                if( dogHandler(params[1]) == 0 ) { 
                    //all good.
                }
            }
            else{printf("dog command broke. make sure you're only using one param.\n");}
            break;
        case LS:
            if( nparams == 1 ) {
                if( ls(params[1]) == 0 ) {
                    //all good
                }
            }
            else {printf("ls command broke. make sure your ls is naked.\n");}
            break;

        case PWD:
            if(nparams==1){
                if(pwd()==0){
                    //all good
                }
            }
            else{printf("pwd command broke. make sure it is naked.\n");}

        default:
            printf("Invalid command!\n");
    }   

    return rc; 
}

int cd(char* input){

    if (fork() == 0){
        execl("/bin/cd", "cd", NULL);
    }
    else{
        //Parent stuff happens here.
    }
    printf("cd function fired!\n");
    return 0;
}

int pwd(){

    if (fork() == 0){
        execl("/bin/pwd", "pwd", NULL);
        exit(0);
    }
    else{
        //Parent stuff happens here.
    }

    printf("pwd function fired!\n");
    return 0;

}

int catHandler(char* input){
    printf("cat function fired!\n");
    return 0;

}

int dogHandler(char* input){
    printf("dog function fired!\n");
    return 0;

}

int ls(char* input) {

    if (fork() == 0){
        execl("/bin/ls", "ls", NULL);
        exit(0);
    }
    else{
        //Parent stuff happens here.
    }

    return 0;
}
