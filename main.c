#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "main.h"
#include <signal.h>
#include <sys/wait.h>
#define MAX_COMMAND_LENGTH 100
#define MAX_PARAMETER_LENGTH 30
#define MAX_PARAMS 10

//int global pipeFlag = 0;
int status = -900;
int pipeFlag=0;
int backgroundFlag=0;
void signal_handler(){
    printf("Use CTR+D to kill, silly goose!\n");
}


//function declarations here so the warnings shut up.

enum cmds {CD=0, EXIT, CAT, GUSTY, LS, PWD, DATE, HELP};

char *cmdstr[] = {"cd","exit","cat","gusty", "ls", "pwd", "date", "help"};

int main()
{   
    char cmd[MAX_COMMAND_LENGTH + 1];
    char* params[MAX_PARAMS+1];
    int cmdCount=0,nparams=0;
    struct sigaction new, old;


    //sigaction stuff here...
    new.sa_handler = signal_handler;
    new.sa_flags = SA_RESTART;
    sigemptyset(&new.sa_mask);
    if (sigaction(SIGINT, &new, &old) !=0){
        return(1);
    }

    //hopefully this takes the old handler for SIGINT (ctrl+c)
    //and copies it over to SIGQUIT (ctr+d)
    if ( sigaction(SIGQUIT, &old, NULL) !=0 ){

        return(1);
    }


    while(1){
        nparams=0;
        pipeFlag=0;
        if(status==0){

            if(backgroundFlag==1){

                while(1){}
            }
            else{
                exit(0);
            }
        }
        backgroundFlag=0;
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

int date(char *input){
   if(fork()==0){
        execl("/bin/date", "date", NULL);
        perror("date failed.\n");
        exit(1);
    }
    else{
        //Parent stuff here.
        wait(NULL);
    }

    return 0;
}

int gustyHandler(char* input){
    printf("gusty function fired! Alright!!\n %s \n",input);
    if(fork() == 0) {
        execl("/bin/cat", "cat", "gusty.txt", NULL);
        perror("gusty error");
        exit(1);
    } else {
        wait(NULL);
    }
    return 0;

}

int executeCmd(char** params, int nparams)
{
    int rc = 1;
    int ncmds = sizeof(cmdstr) / sizeof(char *); 
    int cmd_index;
    for (cmd_index = 0; cmd_index < ncmds; cmd_index++)
        if (strcmp(params[0], cmdstr[cmd_index]) == 0)
            break;

    //set default input data for the case that we don't need to pipe:
    char inputdata [MAX_PARAMETER_LENGTH];
    if(params[1] != NULL) { 
        strcpy(inputdata ,params[1]);
    }
    char inputbuffer[MAX_PARAMETER_LENGTH];
    //now we check to find if we will need to pipe:
    int word;
    //int data;
    for(word=0; word < ncmds; word++){
        if(params[word]==NULL){break;}
        if(strcmp(params[word],"|")==0){
            //in here, we know that we have a pipe.
            pipeFlag=1;
            int fd[2];
            pipe(fd);
            //at this point, we have openned the pipes.
            //the pipes are prepared for use.
            //pipeFlag=1;
            //this will signal the other functions that they are piping.
            if(pipe(fd) < 0) {
                perror("pipe");
                exit(1);
            }
            status=fork();
            if(status == 0) {
                //child (first command)
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                //data = 1;
                strcpy(inputdata,params[1]);
            } else {
                //Parent (second command)
                for (cmd_index = cmd_index; cmd_index < ncmds; cmd_index++) {
                    if (strcmp(params[word+1], cmdstr[cmd_index]) == 0) {
                        break;
                    }
                }
                //wait(NULL);
                //must gather data from child before continuing.
                //dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                read(fd[0] , inputbuffer , MAX_PARAMETER_LENGTH -1 ); 
                //printf("sanity check.. inputdata = %s , inputbuffer = %s\n", inputdata,inputbuffer); 
                strcpy(inputdata,inputbuffer);
            }
        }
        if(strcmp(params[nparams-1], "&") == 0) {
            // run process in the background
            printf("Process running in background\n");
            status=fork();
            if(status==0){
                backgroundFlag=1;
            }
        } else {
            wait(NULL);
            break;
        }
    }

    switch (cmd_index){
        case CD: 
            if(nparams >1){ 
                if(cd(inputdata) == 0){ 
                    //if cd() returns 0, that means everything is ok :)
                }
            }
            else{printf("cd command broke, that's your fault loser.\n");}
            break;

        case EXIT:
            rc=0;
            break;

        case CAT:
            if( nparams > 1 ){
                if( catHandler(inputdata) == 0){ 
                    //all good.
                }
            }
            else{printf("cat command broke. make sure you are using only one param.\n");}
            break;

        case GUSTY:
            if( nparams > 0){ 
                if( gustyHandler("gusty.txt") == 0 ) { 
                    //all good.
                }

            }
            else{printf("gusty command broke. make sure you're only using one param.\n");}
            break;
        case LS:
            if( nparams > 0) {
                if( ls(inputdata) == 0 ) {
                    //all good
                }
            }
            else {printf("ls command broke. make sure your ls is naked.\n");}
            break;

        case PWD:
            if(nparams > 0){
                if(pwd()==0){
                    //all good
                }
            }
            else{printf("pwd command broke. make sure it is naked.\n");}
            break;

        case DATE:
            if(nparams>0){
                if(date(inputdata)==0){
                    //all good
                }
            }
            else{printf("date command broke.\n");}
            break;

        case HELP:
            printf("Commands: cd, exit, cat, gusty, ls, pwd, date, help\n");
            break;

        default:
            printf("Invalid command!\n");
    }   
    return rc; 
}

int cd(char* input){

    printf("input: %s\n", input);
    if (fork() == 0){
        //execl("/bin/cd", "cd", NULL);
        if(strcmp(input, "home") == 0) {
            if(chdir(getenv("HOME")) == -1) {
                perror("chdir failed");
                exit(EXIT_FAILURE);
            }

            printf("Changed directory to %s\n", input);
        } else {
            if(chdir(input) == -1) {
                perror("chdir failed");
                exit(EXIT_FAILURE);
            }

            printf("Changed directory to %s\n", input);
        }
    }
    else{
        //Parent stuff happens here.
        wait(NULL);
    }
    //printf("cd function fired!\n");
    return 0;
}

int pwd(){

    if (fork() == 0){
        execl("/bin/pwd", "pwd", NULL);
    }
    else{
        wait(NULL);
        //Parent stuff happens here.
    }

    //printf("pwd function fired!\n");
    return 0;

}

int catHandler(char* input){

    if(fork()==0){
        //child process here
        execl("/bin/cat","cat",input,NULL);
        //below lines will not execute if execl() succeeds.
        perror("cat failed.");
        exit(1);
        //these will force exit.
    }
    else{

        //parent process here
        wait(NULL);

    }
    //printf("cat function fired!\n");
    return 0;

}

int ls(char* input) {

    if (fork() == 0){
        execl("/bin/ls", "ls", NULL);
        perror("ls failed.\n");
        exit(1);
    }
    else{
        wait(NULL);
        //Parent stuff happens here.
    }

    return 0;
}
