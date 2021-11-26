/*  @title: Complete Shell
	@author: Jesus Gutierrez
	@language: C
	@lastModified: Sep 24,2021 */


/* Declaring Header Files */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

/* Declaring Functions */

void global_var(); 
void environmment();
void set_environment_variables();
void directory();
void token_space (char *);
void tokenize_redirect_input_output(char *);
void tokenize_redirect_input(char *);
void tokenize_redirect_output(char *);
static int execute_inbuild_commands(char *, int, int, int);
void token_pipe ();
static int execute(int, int, int, char *);
void shell_prompt(); 
void interruptHandler(int );
void check_process ();
char *white_spaces (char* );

/* Declaring Global Variables */

#define MAX_INPUT_SIZE 1024
#define MAX_CHAR_SIZE 512

pid_t pid;
int fd;
char cwd[MAX_INPUT_SIZE];
static char *args[MAX_CHAR_SIZE];
static char prompt[MAX_CHAR_SIZE];
char *cmd_exec[MAX_CHAR_SIZE];
int flag, len;
int bckgrnd_flag;
char *input_buffer;
int environmment_flag;
int flag_without_pipe;
int output_redirection;
int input_redirection;
int status;
char *input_redirection_file;
char *output_redirection_file;

/* Initializing Global Variables */

void global_var() {
	fd = 0;
	flag = 0;
	len = 0;
	cwd[0] = '\0';
	prompt[0] = '\0';
 	pid = 0;
	environmment_flag = 0;
	flag_without_pipe = 0;
	output_redirection = 0;
	input_redirection = 0;
}


/* Interrupt Signals Handler Function */

void interruptHandler(int sig_num) {

    signal(SIGINT, interruptHandler);
    fflush(stdout);
    return;
}

  
/* Shell Prompt Function */

void shell_prompt() {

	if (getcwd(cwd, sizeof(cwd)) != NULL) {

		strcpy(prompt, "minershell: ");
		//strcat(prompt, cwd);
		strcat(prompt, "$ ");
	}
	else {

		perror("Current working directory ERROR: ");
	}
	return;
}

/* Change Directory Function */

void directory() {

	char *home_dir = "/home";

	if ((args[1]==NULL) || (!(strcmp(args[1], "~") && strcmp(args[1], "~/"))))
		chdir(home_dir);
	else if (chdir(args[1]) < 0)
		perror("No such file or directory: ");

}

/* Function used to skip any white spaces in the input string */

char *white_spaces (char* str) {

	int i = 0, j = 0;
	char *temp;
	if (NULL == (temp = (char *) malloc(sizeof(str)*sizeof(char)))) {
		perror("Found memory ERROR: ");
		return NULL;
	}

	while(str[i++]) {

		if (str[i-1] != ' ')
			temp[j++] = str[i-1];
	}
	temp[j] = '\0';
	return temp;
}

/* Function used to execute the inbuild commands. */

static int execute_inbuild_commands (char *cmd_exec, int input, int isfirst, int islast) {

	char *new_cmd_exec;

	new_cmd_exec = strdup(cmd_exec);

	token_space (cmd_exec);
	check_process ();

	if (args[0] != NULL) {
		if (!(strcmp(args[0], "exit") && strcmp(args[0], "quit")))
			exit(0);

		if (!strcmp("cd", args[0])) {

			directory();
			return 1;
		}

		
	}
	return (execute(input, isfirst, islast, new_cmd_exec));
}

/* This function is used to parse the input when both input redirection 
   ["<"] and output redirection [">"] are present */

void tokenize_redirect_input_output (char *cmd_exec) {

	char *val[128];
	char *new_cmd_exec, *s1, *s2;
	new_cmd_exec = strdup(cmd_exec);

	int m = 1;
	val[0] = strtok(new_cmd_exec, "<");
	while ((val[m] = strtok(NULL,">")) != NULL) m++;

	s1 = strdup(val[1]);
	s2 = strdup(val[2]);

	input_redirection_file = white_spaces(s1);
	output_redirection_file = white_spaces(s2);

	token_space (val[0]);
	return;
}

/* Function parses input for only input redirection ["<"] */

void tokenize_redirect_input (char *cmd_exec) {

	char *val[128];
	char *new_cmd_exec, *s1;
	new_cmd_exec = strdup(cmd_exec);

	int m = 1;
	val[0] = strtok(new_cmd_exec, "<");
	while ((val[m] = strtok(NULL,"<")) != NULL) m++;

	s1 = strdup(val[1]);
	input_redirection_file = white_spaces(s1);

	token_space (val[0]);
	return;
}

/* Function parses input for only output redirection [">"] */

void tokenize_redirect_output (char *cmd_exec) {

	char *val[128];
	char *new_cmd_exec, *s1;
	new_cmd_exec = strdup(cmd_exec);

	int m = 1;
	val[0] = strtok(new_cmd_exec, ">");
	while ((val[m] = strtok(NULL,">")) != NULL) m++;

	s1 = strdup(val[1]);
	output_redirection_file = white_spaces(s1);

	token_space (val[0]);
	return;
}

/* Function creates pipe and executes non-inbuilt commands (execvp) */

static int execute (int input, int first, int last, char *cmd_exec) {

	int mypipefd[2], ret, input_fd, output_fd;

	if (-1 == (ret = pipe(mypipefd))) {
		perror("Pipe has given an ERROR: ");
		return 1;
	}

	pid = fork();

	if (pid == 0) {

		if (first == 1 && last == 0 && input == 0) {
			
			dup2 (mypipefd[1], 1);
		}
		else if (first == 0 && last == 0 && input != 0) {

			dup2 (input, 0);
			dup2 (mypipefd[1], 1);
		}
		else {

			dup2 (input, 0);
		}

		if (strchr(cmd_exec, '<') && strchr(cmd_exec, '>')) {

			input_redirection = 1;
			output_redirection = 1;
			tokenize_redirect_input_output (cmd_exec);
		}
		else if (strchr(cmd_exec, '<')) {

			input_redirection = 1;
			tokenize_redirect_input (cmd_exec);
		}
		else if (strchr(cmd_exec, '>')) {

			output_redirection = 1;
			tokenize_redirect_output (cmd_exec);
		}

		if (output_redirection) {

			if ((output_fd = creat(output_redirection_file, 0644)) < 0) {

				fprintf(stderr, "Failed to open %s for writing\n", output_redirection_file);
				return (EXIT_FAILURE);
			}
			dup2 (output_fd, 1);
			close (output_fd);
			output_redirection = 0;
		}

		if (input_redirection) {

			if ((input_fd = open(input_redirection_file, O_RDONLY, 0)) < 0) {

				fprintf(stderr, "Failed to open %s for reading\n", input_redirection_file);
				return (EXIT_FAILURE);
			}
			dup2 (input_fd, 0);
			close (input_fd);
			input_redirection = 0;
		}
		
		check_process ();

		if (execvp(args[0], args) < 0) {
			fprintf(stderr, "%s: Command not found\n",args[0]);
		}
		exit(0);
	}

	else {

		if (bckgrnd_flag == 0)
			waitpid(pid,0,0);
	}

	if (last == 1)
		close(mypipefd[0]);

	if (input != 0)
		close(input);

	close(mypipefd[1]);
	return (mypipefd[0]);
}

/* This function tokenizes the input string based on white-space [" "] */

void token_space (char *str) {

	int m = 1;

	args[0] = strtok(str, " ");
	while ((args[m] = strtok(NULL," ")) != NULL) m++;
	args[m] = NULL;
}

/* Function checks if process needs to run in the background/foreground */

void check_process () {

	int i = 0;
	bckgrnd_flag = 0;
	
	while (args[i] != NULL) {
		if (!strcmp(args[i], "&")) {
			bckgrnd_flag = 1;
			args[i] = NULL;
			break;
		}
		i++;
	}

}

/* This function tokenizes the input string based on pipe ["|"] */

void token_pipe() {

	int i, n = 1, input = 0, first = 1;

	cmd_exec[0] = strtok(input_buffer, "|");
	while ((cmd_exec[n] = strtok(NULL, "|")) != NULL) n++;

	cmd_exec[n] = NULL;
	
	for (i = 0; i < n-1; i++) {

		input = execute_inbuild_commands(cmd_exec[i], input, first, 0);	
		first = 0;
	} 

	input = execute_inbuild_commands(cmd_exec[i], input, first, 1);
	return;
}

/* Main Function */

int main() {

	int status;
	system ("clear");
	signal(SIGINT, interruptHandler);
	char new_line = 0;

	do {

		global_var();
		shell_prompt();
		input_buffer = readline (prompt);

		if (!(strcmp(input_buffer, "\n") && strcmp(input_buffer,"")))
			continue;

		if (!(strncmp(input_buffer, "exit", 4) && strncmp(input_buffer, "quit", 4))) {

			flag = 1;
			break;
		}

		token_pipe();

		if (bckgrnd_flag == 0)
			waitpid(pid,&status,0);
		else
			status = 0;

	} while(!WIFEXITED(status) || !WIFSIGNALED(status));

	if (flag == 1) {

		printf("\nMinershell Closed\n");
		exit(0);
	}

	return 0;
}
