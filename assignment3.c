#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

/*
 * CS3305A - Assignment 3
 * October 25th, 2021
 * Rishabh Jain
 */

// Creating global array to be used by sprintf in main and functions
char output[60];

// Pipe
int port[2];

char array[0];

int pipeSize;
int S;

// This function is executed by thread 100
void *sum(void *thread_id) {
    int *id = (int*)thread_id;

    // Thread reads from pipe
    read(port[0], array, pipeSize);

    // Get X and Y from the array by splitting around ","
    char * xToken = strtok(array, ",");
    char * yToken = strtok(NULL, ",");

    // Convert X and Y to integers
    int X = atoi(xToken) , Y = atoi(yToken);

    // Print the statement and the values
    sprintf(output, "thread (TID %d) reads X = %d and Y = %d from the pipe", *id, X, Y);
    puts(output);

    // Adds X and Y
    S = X + Y;
    sprintf(output, "%d", S);

    pipeSize = strlen(output);
    // Write to pipe and print statement
    write(port[1], output, pipeSize);
    sprintf(output, "thread (TID %d) writes X + Y = %d to the pipe", *id, S);
    puts(output);
    return 0;
}

// This function is executed by thread 101
void *odd_even(void *thread_id) {
    int *id = (int*)thread_id;

    // Thread reads from pipe
    read(port[0], output, pipeSize);
    // Convert S to int
    S = atoi(output);

    // Print the statement and the values
    sprintf(output, "thread (TID %d) reads X + Y = %d from the pipe", *id, S);
    puts(output);

    // Check if S is even or odd
    if (S % 2 == 0) {
        sprintf(output, "thread (TID %d) identifies X + Y = %d as an even number", *id, S);
        puts(output);
    }
    else {
        sprintf(output, "thread (TID %d) identifies X + Y = %d as an odd number", *id, S);
        puts(output);
    }

    sprintf(output, "%d", S);
    pipeSize = strlen(output);
    // Write to pipe and print statement
    write(port[1], output, pipeSize);
    sprintf(output, "thread (TID %d) writes X + Y = %d to the pipe", *id, S);
    puts(output);

    return 0;
}

// This function is executed by thread 102
void *digit_count(void *thread_id) {
    int *id = (int*)thread_id;

    // Thread reads from pipe
    read(port[0], output, pipeSize);
    // Convert S to int
    S = atoi(output);
    int sCount = S;

    // Print the statement and the values
    sprintf(output, "thread (TID %d) reads X + Y = %d from the pipe", *id, S);
    puts(output);

    // Count the number of digits in S
    int count = 0;
    while (sCount != 0) {
        sCount = sCount / 10;
        ++count;
    }

    // Print the statement and the values
    sprintf(output, "thread (TID %d) identifies X + Y = %d as a %d digit number", *id, S, count);
    puts(output);

    return 0;
}

int main(int argc, char *argv[2]) {
    // Parent PID
    int parent_pid = getpid();

    // Pipe error
    if (pipe(port) < 0) {
        perror("pipe error");
        exit(0);
    }

    // Accepts two integers from the user as command-line arguments and stores them in variables X and Y

    char *xArg = argv[1];
    int X = atoi(xArg);
    char *yArg = argv[2];
    int Y = atoi(yArg);

    // Array to hold the two integer arguments


    // Print the received values
    sprintf(output, "parent (PID %d) receives X = %d and Y = %d from the user", parent_pid, X, Y);
    puts(output);

    // Parent writes X to the pipe
    sprintf(output, "%d,%d", X, Y);
    pipeSize = strlen(output);
    write(port[1], output, strlen(output));
    sprintf(output, "parent (PID %d) writes X = %d and Y = %d to the pipe", parent_pid, X, Y);
    puts(output);

    // Thread IDs
    pthread_t thread_1, thread_2, thread_3;
    int thread_id_1 = 100, thread_id_2 = 101, thread_id_3 = 102;
    // Thread 1 (ID 100)
    if (pthread_create(&thread_1, NULL, sum, &thread_id_1)) {
        sprintf(output, "Error while creating thread 1\n");
        exit(1);
    }
    pthread_join(thread_1, NULL);
    // Thread 2 (ID 101)
    if (pthread_create(&thread_2, NULL, odd_even, &thread_id_2)) {
        sprintf(output, "Error while creating thread 2\n");
        exit(1);
    }
    pthread_join(thread_2, NULL);
    // Thread 3 (ID 102)
    if (pthread_create(&thread_3, NULL, digit_count, &thread_id_3)) {
        sprintf(output, "Error while creating thread 3\n");
        exit(1);
    }
    pthread_join(thread_3, NULL);

}