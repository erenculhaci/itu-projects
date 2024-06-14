/* Usage: client <filename>
 * Description: This program reads a list of URLs from a file and sends HTTP
 * requests to the specified hosts and ports. It then reads the HTTP responses
 * and prints them out.
*/

#include "blg312e.h"

#define MAX_THREADS 100
#define MAX_URL_LENGTH 1024

typedef struct { // Request information
    char *host; // Hostname
    int port; // Port number
    char *filename; // Filename
} request_info_t;

pthread_t threads[MAX_THREADS]; // Thread identifiers
int thread_count; // Number of threads

/*
 * Send an HTTP request for the specified file 
 */
void clientSend(int fd, char *filename, char *hostname) {
    char buf[MAXLINE]; // Buffer for the HTTP request

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\r\n", filename); // Form the HTTP request
    sprintf(buf, "%sHost: %s\r\n\r\n", buf, hostname); // Form the HTTP request
    Rio_writen(fd, buf, strlen(buf)); // Send the HTTP request
}

/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd) {
    rio_t rio; // Rio buffer for reading from the socket
    char buf[MAXBUF];   // Buffer for reading from the socket
    int length = 0; // Length of the HTTP body
    int n; // Number of bytes read
  
    Rio_readinitb(&rio, fd); // Initialize the Rio buffer

    /* Read and display the HTTP Header */
    n = Rio_readlineb(&rio, buf, MAXBUF); // Read the first line of the HTTP response
    while (strcmp(buf, "\r\n") && (n > 0)) {    // While the line is not empty
        printf("Header: %s", buf); // Print the line
        n = Rio_readlineb(&rio, buf, MAXBUF); // Read the next line

        /* If you want to look for certain HTTP tags... */
        if (sscanf(buf, "Content-Length: %d ", &length) == 1) { // Look for the Content-Length tag
            printf("Length = %d\n", length); // Print the length
        }
    }

    /* Read and display the HTTP Body */
    n = Rio_readlineb(&rio, buf, MAXBUF); // Read the first line of the HTTP body
    while (n > 0) { // While there are lines to read
        printf("%s", buf); // Print the line
        n = Rio_readlineb(&rio, buf, MAXBUF); // Read the next line
    }

}

void *request_handler(void *arg) { // Thread routine
    request_info_t *info = (request_info_t *)arg; // Request information
    int clientfd; // Client socket descriptor

    // Open a connection to the specified host and port
    clientfd = Open_clientfd(info->host, info->port);
    
    // Send HTTP request for the specified file
    clientSend(clientfd, info->filename, info->host);
    
    // Read and print the HTTP response
    clientPrint(clientfd);
    
    Close(clientfd); // Close the connection
    return NULL; // Exit the thread
}

int main(int argc, char *argv[]) {
    FILE *file; // File pointer for the input file
    char line[MAX_URL_LENGTH]; // Buffer for reading lines from the file
    char *host, *filename; // Hostname and filename extracted from the line
    int port; // Port number extracted from the line
    int i; // Loop counter

    if (argc != 2) { // Check for the correct number of arguments
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]); // Print usage
        exit(1);    // Exit
    }

    file = fopen(argv[1], "r"); // Open the input file
    if (file == NULL) {  // Check for errors
        perror("Error opening file"); // Print an error message
        exit(1);   // Exit
    }

    while (fgets(line, sizeof(line), file)) { // Read a line from the file
        // Remove newline character from the end
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character

        // Parse the line
        host = strtok(line, " "); // Extract the hostname
        port = atoi(strtok(NULL, " ")); // Extract the port number
        filename = strtok(NULL, " "); // Extract the filename
        
        request_info_t *info = (request_info_t *)malloc(sizeof(request_info_t)); // Allocate memory for the request information
        info->host = strdup(host); // Allocate memory and copy the hostname
        info->port = port; // Set the port number
        info->filename = strdup(filename); // Allocate memory and copy the filename


        // Create a thread for the request
        pthread_create(&threads[thread_count++], NULL, request_handler, (void *)info);
    }

    fclose(file); // Close the input file

    // Wait for all threads to finish
    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    exit(0);   // Exit the program
}
