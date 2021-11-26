# Shell Part 1

The shells you implement will be similar to, but simpler than, the one you run every day in Unix. You will be building a simple shell to run simple Linux commands. A shell takes in user input, forks one or more child processes using the fork system call, calls exec from these children to execute user commands, and reaps the dead children using the wait system call. Your shell must execute all simple Linux commands like ls, wc, cat, echo, pwd and sleep. These commands are readily available as executables on Linux, and your shell must simply invoke the existing executable. It is important to note that you must implement the shell functionality yourself, using the fork, exec, and wait system calls. You must not use library functions like system which implement shell commands by invoking the Linux shell—doing so defeats the purpose of this assignment!
To implement this task, your job is to simply invoke exec on any command that the user gives as input. If the Linux command execution fails due to incorrect arguments, an error message will be printed on screen by the executable, and your shell must move on to the next command. If the command itself does not exist, then the exec system call will fail, and you will need to print an error message on screen and move on to the next command.
Your shell must gracefully handle errors. An empty command (typing return) should simply cause the shell to display a prompt again without any error messages. For all incorrect commands or any other erroneous input, the shell itself should not crash. It must simply notify the error and move on to prompt the user for the next command.
For all commands, you must take care to terminate and carefully reap any child process the shell may have spawned. Please verify this property using the ps command during testing. When the forked child calls exec to execute a command, the child automatically terminates after the executable completes. However, if the exec system call failed for some reason, the shell must ensure that the child is terminated suitably. When not running any command, there should only be the one main shell process running in your system, and no other children.

# Shell Part 2

Enabling File Redirection in your Shell
Many times, a shell user prefers to send the output of a program to a file rather than to the screen. Usually, a shell provides this nice feature with the “>” (greater than symbol) character. Formally this is named as redirection of standard output. To make your shell users happy, your shell should also include this feature, but with a slight twist (explained below).
For example, if a user types “ls -la /tmp > output”, nothing should be printed on the screen. Instead, the standard output of the ls program should be rerouted to the file output. In addition, the standard error output of the program should be rerouted to the file output (the twist is that this is a little different than standard redirection).
If the output file exists before you run your program, you should simple overwrite it (after truncating it).


# Shell Part 3

ow that you have already implemented a basic working shell with file redirection. It’s time to add a new functionality called “pipes”.

Augmenting your “Minershell” with pipelining functionality:
Your task is to modify your previous C-based shell program to implement the pipeline functionality, where you can execute commands with at least one pipe (e.g. ls | wc).
