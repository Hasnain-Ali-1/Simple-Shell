/*
project: 01
author: Hasnain Ali
email: hali6@umbc.edu
student id: jl69013
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"
#include <stdbool.h>

/*
In this project, you are going to implement a number of functions to
create a simple linux shell interface to perform basic linux commands
*/


//DEFINE THE FUNCTION PROTOTYPES
void user_prompt_loop(void);
char* get_user_command(void);
char** parse_command(char *command);
void execute_command(char **args);
void store_input(char *userInput);
void display_history(void);
void proc_command(char **parsedInput);
void exit_command(char **parsedInput);
void deallocate_tokens(int token_count, char **tokens);
void deallocate_memory(char **parsedInput);

int main(int argc, char **argv)
{
    /*
    Write the main function that checks the number of argument passed to ensure 
    no command-line arguments are passed; if the number of argument is greater 
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedly 
    until the user enters the "exit" command.
    */

    if (argc > 1) {
        fprintf(stderr, "Error: Command Line Arguments Passed.\n");
    }
    else {
        user_prompt_loop();
    }
    // Resolves the unused variable warning for argv, per Prof Sebald in chat
    (void)argv;
    return 0;
}

/*
user_prompt_loop():
Get the user input using a loop until the user exits, prompting the user for a command.
Gets command and sends it to a parser, then compares the first element to the two
different commands ("/proc", and "exit"). If it's none of the commands, 
send it to the execute_command() function. If the user decides to exit, then exit 0 or exit 
with the user given value.
*/

void user_prompt_loop(void) {
    // initialize variables

    /*
    loop:
        1. prompt the user to type command by printing >>
        2. get the user input using get_user_command() function 
        3. parse the user input using parse_command() function 
        Example: 
            user input: "ls -la"
            parsed output: ["ls", "-la", NULL]
        4. compare the first element of the parsed output to "/proc"and "exit"
        5. if the first element is "/proc" then you have the open the /proc file system 
           to read from it
            i) concat the full command:
                Ex: user input >>/proc /process_id_no/status
                    concated output: /proc/process_id_no/status
            ii) read from the file line by line. you may user fopen() and getline() functions
            iii) display the following information according to the user input from /proc
                a) Get the cpu information if the input is /proc/cpuinfo
                - Cpu Mhz
                - Cache size
                - Cpu cores
                - Address sizes
                b) Get the number of currently running processes from /proc/loadavg
                c) Get how many seconds your box has been up, and how many seconds it has been idle from /proc/uptime
                d) Get the following information from /proc/process_id_no/status
                - the vm size of the virtual memory allocated the vbox 
                - the most memory used vmpeak 
                - the process state
                - the parent pid
                - the number of threads
                - number of voluntary context switches
                - number of involuntary context switches
                e) display the list of environment variables from /proc/process_id_no/environ
                f) display the performance information if the user input is /proc/process_id_no/sched
        6. if the first element is "exit" the use the exit() function to terminate the program
        7. otherwise pass the parsed command to execute_command() function 
        8. free the allocated memory using the free() function
    */

    /*
    Functions you may need: 
        get_user_command(), parse_command(), execute_command(), strcmp(), strcat(), 
        strlen(), strncmp(), fopen(), fclose(), getline(), isdigit(), atoi(), fgetc(), 
        or any other useful functions
    */

    bool exitShell = false;
    while (exitShell == false) {
	// Gets the user input
        char *userInput = get_user_command();
	// Adds the input to history
	store_input(userInput);
	// Parse the user input to the correct format
	char **parsedInput = parse_command(userInput);
	free(userInput);
	// If there was an error in parsing/unescaping, reprompt
	if (parsedInput == NULL) {
	    exitShell = false;
	}
	else {
	    // If exit was the command entered
            if (strcmp(parsedInput[0], "exit") == 0) {
		exit_command(parsedInput);
	    	exitShell = false;
	    }
	    // If history was the command entered
            else if (strcmp(parsedInput[0], "history") == 0) {
		// Checks to see there were no other arguments
		if (parsedInput[1] == NULL) {
                    display_history();
		}
		else {
		    printf("Too many arguments for history command\n");
		}
		deallocate_memory(parsedInput);
                exitShell = false;
            }
	    // If /proc was entered
	    else if (strcmp(parsedInput[0], "/proc") == 0) {
		proc_command(parsedInput);
		deallocate_memory(parsedInput);
		exitShell = false;
	    }
	    // Attempt to execute the given user input
	    else {
	    	execute_command(parsedInput);
	     	deallocate_memory(parsedInput);
	    	exitShell = false;
            }
	}
    }
}

/*
get_user_command():

Take input of arbitrary size from the user and return to the user_prompt_loop()
*/

char *get_user_command() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    printf(">> ");
    read = getline(&line, &len, stdin);
    // Gets rid of newline character
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }
    return line;
}

// Stores user input in .421sh
void store_input(char *userInput) {
    char *filename = ".421sh";
    char *home = getenv("HOME");
    // Error handling
    if (home == NULL) {
	perror("Unable to get home directory");
	return;
    }
    // Get the buffer size for the full path
    size_t size = strlen(home) + strlen(filename) + 2;
    char *filepath = (char *)malloc(size);
    snprintf(filepath, size, "%s/%s", home, filename);
    // Attempt to open the file
    FILE *file = fopen(filepath, "a");
    // Error handling
    if (file == NULL) {
	perror("Unable to open .421sh");
	free(filepath);
	return;
    }
    // Appends to the file
    fprintf(file, "%s\n", userInput);
    fclose(file);
    free(filepath);
}

// Displays the 10 most recent items .421sh, or the whole file if there are less
// than 10 commands in .421sh
void display_history() {
    // Opens the file
    char *filename = ".421sh";
    char *home = getenv("HOME");
    // Error handling
    if (home == NULL) {
        perror("Unable to get home directory");
        return;
    }
    // Get the buffer size for the full path
    size_t size = strlen(home) + strlen(filename) + 2;
    char *filepath = (char *)malloc(size);
    // Error handling
    if (filepath == NULL) {
        perror("Memory allocation error");
	free(filepath);
        return;
    }
    snprintf(filepath, size, "%s/%s", home, filename);
    FILE *file = fopen(filepath, "r");
    // Error handling
    if (file == NULL) {
        perror("Unable to open .421sh");
        free(filepath);
        return;
    }
    // Once the file has been opened, counts the number of lines in the file
    int totalLines = 0;
    char *line = NULL;
    size_t lineLength = 0;
    ssize_t bytesRead;
    while ((bytesRead = getline(&line, &lineLength, file)) != -1) {
        totalLines++;
    }
    // Calculate the line that is the first of the last 10 lines.
    // e.g. If there are 15 lines, we want to start storing once we are at line 5
    int startStoring = totalLines - 10;
    // If there are less than 10 lines, we'll store all the lines in the file
    if (startStoring < 0) {
        startStoring = 0;
    }
    // Create an array to store the last 10 lines.
    char *commands[10];
    // Keeps track of the current index of the array we are storing commands into
    int index = 0;
    // Reset the file pointer to the beginning.
    rewind(file);
    char *command = NULL;
    size_t commandLength = 0;
    // Represents the number of lines we read in, will be used to know when
    // to start storing lines in our array
    int linesRead = 0;
    while (getline(&command, &commandLength, file) != -1) {
	// Once we reach the line where we want to store commands
        if (linesRead >= startStoring) {
            // Allocate memory for the line and copy it.
            commands[index] = strdup(command);
	    // Error handling
            if (commands[index] == NULL) {
                perror("Memory allocation error");
                break; // Exit the loop if allocation fails.
            }
	    // Increment index because it now has data in it
            index++;
        }
        // Increment linesRead because we have gone thorugh one line
        linesRead++;
    }
    // Prints the array containing the last 10 lines and deallocates it
    for (int i = 0; i < index; i++) {
        printf("%s", commands[i]);
        free(commands[i]);
    }
    // Deallocates memory and closes the file
    free(command);
    free(line);
    fclose(file);
    free(filepath);
    return;
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
Example: 
    user input: "ls -la"
    parsed output: ["ls", "-la", NULL]
Example: 
    user input: "echo     hello                     world  "
    parsed output: ["echo", "hello", "world", NULL]
*/


char** parse_command(char* command) {
    char **tokens = NULL;
    int token_count = 0;
    // Split on spaces and whitespaces characters
    char *token = strtok(command, " \t\n\r");
    while (token != NULL) {
	// If the current token starts with a quote
        if (token[0] == '\'' || token[0] == '\"') {
            // Combine the tokens in the same quote
            char *combined_token = strdup(token + 1);
	    // Error handling
            if (combined_token == NULL) {
                perror("Memory allocation error");
                // Deallocate any previously allocated memory
		deallocate_tokens(token_count, tokens);
                return NULL;
            }
            // Find the closing quote.
            while (token[strlen(token) - 1] != '\'' && token[strlen(token) - 1] != '\"') {
                token = strtok(NULL, " \t\n\r");
		// Error handling
                if (token == NULL) {
                    free(combined_token);
                    printf("Mismatched quotes\n");
		    // Deallocate any previously allocated memory
		    deallocate_tokens(token_count, tokens);
                    return NULL;
                }
                // Append the space and the next token to the combined token.
                combined_token = realloc(combined_token, strlen(combined_token) + strlen(token) + 2);
                strcat(combined_token, " ");
                strcat(combined_token, token);
            }
            // Remove the closing quote.
            combined_token[strlen(combined_token) - 1] = '\0';
	    // Add the combined token to the array
            tokens = realloc(tokens, (token_count + 1) * sizeof(char*));
	    // Error handling
            if (tokens == NULL) {
		free(combined_token);
                perror("Memory allocation error");
                // Deallocate any previously allocated memory
		deallocate_tokens(token_count, tokens);
                return NULL;
            }
            tokens[token_count] = combined_token;
        }
	// If it is a regular token with no quotes
	else {
            tokens = realloc(tokens, (token_count + 1) * sizeof(char*));
	    // Error handling
            if (tokens == NULL) {
                perror("Memory allocation error");
                // Deallocate any previously allocated memory
		deallocate_tokens(token_count, tokens);
                return NULL;
            }
	    // Add it to the array
            tokens[token_count] = strdup(token);
        }
        token_count++;
        token = strtok(NULL, " \t\n\r");
    }
    // Once all tokens have been read in, move to the next empty index and add NULL
    tokens = realloc(tokens, (token_count + 1) * sizeof(char*));
    // Error handling
    if (tokens == NULL) {
        perror("Memory allocation error");
        // Deallocate any previously allocated memory
	deallocate_tokens(token_count, tokens);
        return NULL;
    }
    tokens[token_count] = NULL;
    // Use unescape on the strings in the array
    for (int i = 0; tokens[i] != NULL; i++) {
        char* unescaped_token = unescape(tokens[i], stderr);
	// Error handling
        if (unescaped_token == NULL) {
            fprintf(stderr, "Error while unescaping token\n");
            // Deallocate any previously allocated memory
            deallocate_tokens(token_count, tokens);
            return NULL;
        }
        // Update the array with the unescaped version and free the originals
        free(tokens[i]);
        tokens[i] = unescaped_token;
    }
    return tokens;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/
void execute_command(char **args) {
    // Creates a child process
    pid_t child_pid = fork();
    // If the fork fails, deallocates memory
    if (child_pid == -1) {
	perror("Fork failed");
        deallocate_memory(args);
        return;
    }
    // In the child process
    else if (child_pid == 0) {
	execvp(args[0], args);
	// If execvp failed, deallocate memory and exit the child process
	perror("execvp failed");
	deallocate_memory(args);
	exit(1);
    }
    else {
	int status;
	waitpid(child_pid, &status, 0);
	// Checks for an error in the child process
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                printf("Child process encountered an error with exit status: %d\n", exit_status);
            }
        }
    }
}

// Responsible for the proc command
void proc_command(char **parsedInput) {
    char* command = NULL;
    // Dynamically allocate memory for the concatenated command
    command = (char *)malloc(strlen(parsedInput[0]) + strlen(parsedInput[1]) + 1);
    // Error handling
    if (command == NULL) {
        perror("Memory allocation failed");
	free(command);
        return;
    }
    // Concatenates the user input
    strcpy(command, parsedInput[0]);
    strcat(command, parsedInput[1]);
    // Read from the concatenated path
    FILE* file = fopen(command, "r");
    if (file) {
        char ch;
        while (fread(&ch, 1, 1, file) == 1) {
	    // If character read in is '\0', replaces with '\n'
	    // Some proc files didn't fully print without this
            if (ch == '\0') {
                putchar('\n');
            }
	    else {
                putchar(ch);
            }
        }
        fclose(file);
    }
    else {
        perror("File open failed");
    }
    free(command);
}

// When the user enters exit as their command
void exit_command(char **parsedInput) {
    // If there was a argument provided
    if (parsedInput[1] != NULL) {
        // Checks to see if there are more than one arguments
        if (parsedInput[2] != NULL) {
            printf("Too many arguments for exit command\n");
            deallocate_memory(parsedInput);
        }
        // Checks if the second argument is a number
        else {
            char *endptr;
	    // Attempts to convert the argument to an int
            long int number = strtol(parsedInput[1], &endptr, 10);
	    // If it can't
            if (*endptr != '\0') {
                printf("Invalid argument for exit command\n");
                deallocate_memory(parsedInput);
            }
	    // If it can
            else {
                deallocate_memory(parsedInput);
		printf("Exit with a return status of %ld\n", number);
                exit(number);
            }
        }
    }
    // If no arguments were provided
    else {
        deallocate_memory(parsedInput);
	printf("Exit with a return status of 0\n");
        exit(0);
    }
}

// Deallocates the tokens array used for parsing if an error occurs
void deallocate_tokens(int token_count, char **tokens) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

// Deallocates the array containing parsed input
void deallocate_memory(char **parsedInput) {
    for (int i = 0; parsedInput[i] != NULL; i++) {
        free(parsedInput[i]);
    }
    free(parsedInput);
}
