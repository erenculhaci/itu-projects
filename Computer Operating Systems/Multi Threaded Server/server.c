#include "blg312e.h"
#include "request.h"

#define MAX_THREADS 100 // Maximum number of worker threads
#define MAX_BUFFERS 100 // Maximum number of connection buffers

// Define a structure for the shared connection buffer
typedef struct {
    int connections[MAX_BUFFERS]; // Array of connection descriptors
    int front; // Index of the front of the queue
    int rear; // Index of the rear of the queue
    sem_t mutex; // Semaphore to protect the buffer
    sem_t slots; // Semaphore to count the number of empty slots
    sem_t items; // Semaphore to count the number of filled slots
} connection_buffer_t;

// Global variables
connection_buffer_t conn_buffer; // Shared connection buffer
pthread_t worker_threads[MAX_THREADS]; // Worker threads

// Initialize the connection buffer
void init_connection_buffer(int buffers) { // Initialize the connection buffer
    conn_buffer.front = 0; // Initialize the front of the queue
    conn_buffer.rear = 0; // Initialize the rear of the queue
    sem_init(&conn_buffer.mutex, 0, 1); // Initialize the mutex
    sem_init(&conn_buffer.slots, 0, buffers); // Initialize the slots
    sem_init(&conn_buffer.items, 0, 0); // Initialize the items
}

// Enqueue a connection descriptor
void enqueue_connection(int connfd) { 
    sem_wait(&conn_buffer.slots); // Wait for an empty slot
    sem_wait(&conn_buffer.mutex); // Lock the buffer
    conn_buffer.connections[conn_buffer.rear] = connfd; // Enqueue the connection descriptor
    conn_buffer.rear = (conn_buffer.rear + 1) % MAX_BUFFERS; // Update the rear of the queue
    sem_post(&conn_buffer.mutex); // Unlock the buffer
    sem_post(&conn_buffer.items); // Signal a filled slot
}

// Dequeue a connection descriptor
int dequeue_connection() {
    int connfd; // Connection descriptor
    sem_wait(&conn_buffer.items); // Wait for a filled slot
    sem_wait(&conn_buffer.mutex); // Lock the buffer
    connfd = conn_buffer.connections[conn_buffer.front]; // Dequeue the connection descriptor
    conn_buffer.front = (conn_buffer.front + 1) % MAX_BUFFERS; // Update the front of the queue
    sem_post(&conn_buffer.mutex); // Unlock the buffer
    sem_post(&conn_buffer.slots); // Signal an empty slot
    return connfd; // Return the connection descriptor
}

void *worker_thread(void *arg) { // Worker thread
    while (1) { // Loop forever
        // Dequeue a connection descriptor
        int connfd = dequeue_connection();

        // Handle the request
        requestHandle(connfd);

        // Close the connection
        Close(connfd);
    }
}

// Parse command line arguments
void getargs(int *port, int *threads, int *buffers, int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <port> <threads> <buffers>\n", argv[0]);
        exit(1);
    }

    // Parse port number
    *port = atoi(argv[1]);
    if (*port <= 0) {
        fprintf(stderr, "Invalid port number\n");
        exit(1);
    }

    // Parse number of threads
    *threads = atoi(argv[2]);
    if (*threads <= 0 || *threads > MAX_THREADS) {
        fprintf(stderr, "Invalid number of threads\n");
        exit(1);
    }

    // Parse number of buffers
    *buffers = atoi(argv[3]);
    if (*buffers <= 0 || *buffers > MAX_BUFFERS) {
        fprintf(stderr, "Invalid number of buffers\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) { 
    int listenfd, port, threads, buffers, connfd, clientlen; // Variables
    struct sockaddr_in clientaddr; // Client address

    // Check command line arguments if valid
    if (argc != 4) { 
        fprintf(stderr, "Usage: %s <port> <threads> <buffers>\n", argv[0]);
        exit(1);
    }

    getargs(&port, &threads, &buffers, argc, argv); // Parse command line arguments

    // Initialize connection buffer
    init_connection_buffer(buffers);

    // Create worker threads
    for (int i = 0; i < threads; i++) {
        pthread_create(&worker_threads[i], NULL, worker_thread, NULL);
    }

    // Open listen socket
    listenfd = Open_listenfd(port);

    while (1) { // Loop forever
        clientlen = sizeof(clientaddr); // Initialize the size of the client address
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen); // Accept a connection
        
        // Enqueue the connection descriptor
        enqueue_connection(connfd);
    }

    return 0;
}

