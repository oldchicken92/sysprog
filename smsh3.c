/**  smsh1.c  small-shell version 1
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include	"smsh.h"
#include    <string.h>
#include <fcntl.h>

#define	DFL_PROMPT	"> "

//function to allow for piping
void execute_piping(char *cmdline){
    
    char *args[20];
    int cmd_num = 0;

    char delimeters[] = "|";
    char *token; 
    token = strtok(cmdline, delimeters);

    int i = 0; //incrementer
        while (token != NULL && i < 20){
            args[i] = token;
            i++;
            cmd_num++;
            token = strtok(NULL, delimeters);
        }

    int new_pipe[2];
    int prev_pipe[2];

    for (int j = 0; j < cmd_num; j++){
        if (j < cmd_num - 1){ //check to see if final command or not

            if (pipe(new_pipe) == -1){ // Boring Pipe Failure Stuff
                perror("pipe failed :( ");
                exit(EXIT_FAILURE);
            }
        }

        //use splitline to get all of the command arguments
        char **command_args = splitline(args[j]);

        pid_t pid = fork();

        //for failed fork
        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0){ //child process

        if (j != 0){ //not first command
            dup2(prev_pipe[0],STDIN_FILENO);
            close(prev_pipe[1]);                               
            close(prev_pipe[0]);
        }

        if (j != cmd_num - 1){ //if not final command
                dup2(new_pipe[1], STDOUT_FILENO);
                close(new_pipe[1]);
                close(new_pipe[0]);
        }

        execvp(command_args[0], command_args);
        perror("execvp error");
        exit(EXIT_FAILURE);

        }else{ //parent

            if (j != 0){
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (j != cmd_num - 1){
                prev_pipe[0] = new_pipe[0];
                prev_pipe[1] = new_pipe[1];
            }

            waitpid(pid, NULL, 0);
        }

        free(command_args);

    
    } //end of for loop
    

}

void execute_redirection(char *cmdline){

    char *fileIn = NULL;
    char *fileOut = NULL;

    char *cmd = strdup(cmdline);
    
    //for output
    char *output = strchr(cmd,'>');
    if (output != NULL){
        *output = '\0';
        output++; //get point after >
        fileOut = strtok(output, "\t\n");
    }

    //for input
    char *input = strchr(cmd,'<');
    if (input != NULL){
        *input = '\0';
        input++; //get point after >
        fileIn = strtok(input, "\t\n");
    }

    char **command_args = splitline(cmd);

    pid_t pid = fork();

    //child
    if (pid == 0){
        //output
        if (fileOut != NULL){
            int fd = open(fileOut, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd); //close file descriptor
        }

        //input 
        if (fileIn != NULL){
            int fd = open(fileIn, O_RDONLY);

            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        execvp(command_args[0], command_args);
    }else{
        waitpid(pid, NULL, 0);
    }

    free(cmd);
}

int main()
{
	char	*cmdline, *prompt, **arglist;
	int	result;
	void	setup();

	prompt = DFL_PROMPT ;
	setup();

	while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){

        if (strchr(cmdline, '<') != NULL || (strchr(cmdline, '>'))!= NULL){ 
            
            execute_redirection(cmdline);

        }else if( (strchr(cmdline, '|'))!= NULL) { //if there are no pipes in the command line it goes to original function

            execute_piping(cmdline);

        }else{

            if ( (arglist = splitline(cmdline)) != NULL  ){
			    result = execute(arglist);
			    freelist(arglist);
		    }
            
        }

		
		free(cmdline);
	}
	return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
