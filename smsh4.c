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

void execute_redirection(char *cmdline) {
    char *fileIn = NULL;
    char *fileOut = NULL;

    char *cmd = strdup(cmdline);
    char *original_cmd = cmd; 

    // Handle output
    char *output = strchr(cmd, '>');
    if (output != NULL) {
        *output = '\0';
        output++;
        while (*output == ' ' || *output == '\t') output++;
        fileOut = strtok(output, " \t\n");
    }

    //handle input
    char *input = strchr(cmd, '<');
    if (input != NULL) {
        *input = '\0';
        input++;
        while (*input == ' ' || *input == '\t') input++;
        fileIn = strtok(input, " \t\n");
    }

    char *args[20];
    int cmd_num = 0;
    char *token = strtok(cmd, "|");

    while (token != NULL && cmd_num < 20) {
        while (*token == ' ' || *token == '\t') token++;
        args[cmd_num++] = strdup(token);
        token = strtok(NULL, "|");
    }

    if (cmd_num == 0) {
        free(original_cmd);
        return;
    }

    int pipes[19][2];  // Max 20 commands = 19 pipes
    pid_t pids[20];

    // Create all necessary pipes
    for (int i = 0; i < cmd_num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmd_num; i++) {
        char **command_args = splitline(args[i]);
        if (command_args == NULL) continue;

        pids[i] = fork();

        if (pids[i] == 0) {  // Child

        // Input redirection, first
        if (i == 0 && fileIn != NULL) {
            int fd = open(fileIn, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        //Output redirection, last
        if (i == cmd_num - 1 && fileOut != NULL) {
            int fd = open(fileOut, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        //input
        if (i > 0) {
            dup2(pipes[i - 1][0], STDIN_FILENO);
        }

        //output
        if (i < cmd_num - 1) {
        dup2(pipes[i][1], STDOUT_FILENO);
        }

        for (int j = 0; j < cmd_num - 1; j++) {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }

        execvp(command_args[0], command_args);
        exit(EXIT_FAILURE);
        }

        freelist(command_args);
    }
    for (int i = 0; i < cmd_num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd_num; i++) {
        waitpid(pids[i], NULL, 0);
    }

    for (int i = 0; i < cmd_num; i++) {
        free(args[i]);
    }

    free(original_cmd);
}


int main()
{
	char	*cmdline, *prompt, **arglist;
	int	result;
	void	setup();

	prompt = DFL_PROMPT ;
	setup();

	while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){

        if (strchr(cmdline, '<') != NULL || strchr(cmdline, '>') != NULL || strchr(cmdline, '|') != NULL){ 
            
            execute_redirection(cmdline);

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
