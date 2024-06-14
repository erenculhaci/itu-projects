#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMANDS 20
#define MAX_COMMAND_LENGTH 250
#define MAX_HISTORY_SIZE 20

char last_directory[MAX_COMMAND_LENGTH];
char line[MAX_COMMAND_LENGTH];
char history[MAX_HISTORY_SIZE][MAX_COMMAND_LENGTH];
int history_count = 0;

void execute_command(char *command) {
    char *args[MAX_COMMAND_LENGTH];
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    printf("\n");
    execvp(args[0], args); //termination occurs here but execvp does not fail. It gives no error. The result is correct but the program does not continue.
    perror("execvp failed");
    exit(EXIT_FAILURE);
}


void execute_pipe(char *first_group[], char *second_group[], int num_first_group, int num_second_group) {
    int fd[2];
    pid_t pid;
    int status;
    int i;
    
    if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);

    } else if (pid == 0) { // Child process for first group
        close(fd[0]); // Close unused read end
        dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(fd[1]); // Close pipe write end

        for (i = 0; i < num_first_group; i++) {
            pid_t child_pid = fork(); // Fork for each command in the first group
            if (child_pid == 0) {
                execute_command(first_group[i]);
            } else if (child_pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
        }
        // Wait for all children in the first group to finish
        for (i = 0; i < num_first_group; i++) {
            wait(&status);
        }

        exit(EXIT_SUCCESS); // Exit child process

    } else { // Parent process
        close(fd[1]); // Close unused write end
        dup2(fd[0], STDIN_FILENO); // Redirect stdin to pipe
        close(fd[0]); // Close pipe read end
        for (i = 0; second_group[i] != NULL; i++) {
            execute_command(second_group[i]);
        }
        wait(NULL); // Wait for child process to finish
    }
}

void add_to_history(char *command) {
    if (history_count < MAX_HISTORY_SIZE) {
        strcpy(history[history_count], command);
        history_count++;
    } else {
        // Shift history array to accommodate new command
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[MAX_HISTORY_SIZE - 1], command);
    }
}

void print_history() {
    printf("Command History:\n");
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <batch_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("failed to open file");
        return EXIT_FAILURE;
    }
    
    while (fgets(line, sizeof(line), file)) {
        printf("\n$ %s\n", line); // Print command line
        add_to_history(line);
        if (line[0] == '\n') {
            break;
        }
        line[strcspn(line, "\n")] = 0; // Remove newline
        
        char *pipe_pos = strchr(line, '|'); // Check for pipe symbol "|"
        if (pipe_pos != NULL) { // Pipe symbol found
            // Separate command line into two groups based on pipe symbol "|"
            char *first_group[MAX_COMMANDS];
            int num_first_group = 0;

            char *second_group[MAX_COMMANDS];
            int num_second_group = 0;

            char line_copy[MAX_COMMAND_LENGTH];
            strcpy(line_copy, line);

            char *token = strtok(line, "|");
            char *subtoken = strtok(token, ";");
                while (subtoken != NULL) {
                    first_group[num_first_group++] = subtoken;
                    subtoken = strtok(NULL, ";");
                }

            char *token2 = strtok(line_copy, "|");
            token2 = strtok(NULL, "|");
            char *subtoken2 = strtok(token2, ";");
                while (subtoken2 != NULL) {
                    second_group[num_second_group++] = subtoken2;
                    subtoken2 = strtok(NULL, ";");
                }

            pid_t pid1 = fork();
            if (pid1 == 0) { // Child process
            //Execute first group concurrently and pass the combined output to the second group (after pipe command) as input and execute pipe operation
                execute_pipe(first_group, second_group, num_first_group, num_second_group);
                exit(EXIT_SUCCESS);
            } else if (pid1 < 0) {
                perror("Fork failed");
                return EXIT_FAILURE;
            }
            
        } else {
            // No pipe symbol, simply execute the operations.
            char *token = strtok(line, ";");
            while (token != NULL) {
                char command[MAX_COMMAND_LENGTH];
                strncpy(command, token, sizeof(command));
                if (strncmp(command, "cd ", 3) == 0) { // Check for "cd" command
                    char *directory = command + 3; // Extract directory path
                    directory[strcspn(directory, "\n")] = '\0'; // Remove newline
                    if (chdir(directory) == -1) {
                        perror("chdir failed");
                    } else {
                        strcpy(last_directory, directory); // Update last directory
                    }
                } else if (strncmp(command, "quit", 4) == 0) { // Check for "quit" command
                    fclose(file);
                    return EXIT_SUCCESS;
                } else if (strncmp(command, "history", 7) == 0) { // Check for "history" command
                    print_history();
                } else { // Other commands
                    pid_t pid2 = fork();
                    if (pid2 == 0) { // Child process
                        execute_command(command);
                    } else if (pid2 < 0) { // Error
                        perror("Fork failed");
                        return EXIT_FAILURE;
                    }
                }
                token = strtok(NULL, ";");
             }
         }
         
        while (wait(NULL) > 0); // Wait for all child processes to finish
    }
    fclose(file);
    return EXIT_SUCCESS;
}
