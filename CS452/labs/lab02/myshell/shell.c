#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 80
#define MAXARG 20

void parse_function(char *);

int main()
{
    char cmd[MAXLINE];

    for(; ;)
    {
        printf("mysh$ ");
        fgets(cmd, MAXLINE, stdin);
    	
        if(strcmp(cmd, "exit\n") == 0)
        {
            printf("Now exiting ... good-bye.\n");
            return 0;
        }
        else
        {
            pid_t myprocess = fork();
            
            if(myprocess == 0)
            {
	        printf("Processing your so desired command...\n");
                parse_function(cmd);
                kill(myprocess);
            }
            wait();
        }
        fflush(stdin);
    }
}

void parse_function(char *command)
{
    int i = 0;
    char *argv[MAXARG];
    argv[i++] = strtok(command, "\t \n");
    while ( i < MAXARG && ( argv[i++] = strtok(NULL, "\t \n" ) ) != NULL );
    execvp (argv[0], &argv[0]);
}
