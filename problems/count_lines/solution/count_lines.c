// A simple C program which counts the number of times a line occurs over multiple files, using a
// separate thread to count occurrences in each file.
//
// Written 23/07/2023
// by Daniel Chen (daniel.x.chen@unsw.edu.au)
// for CSESoc COMP1521 Revision Session 23T2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 1024

typedef struct job {
    char *line;
    char *file_path;
    int sum;
} Job;

void *count_in_file(void *job);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <line> [<FILE_TO_SEARCH...>]", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // initialize arrays for thread ids and job data for each file
    pthread_t *thread_ids = malloc(sizeof(pthread_t) * (argc - 2));
    Job *jobs = malloc(sizeof(Job) * (argc - 2));

    // spawn a thread for each file
    for (int i = 0; i < (argc - 2); ++i) {
        jobs[i].line = argv[1];
        jobs[i].file_path = argv[i + 2];
        pthread_create(&thread_ids[i], NULL, count_in_file, &jobs[i]);
    }

    // accumulate results from each thread
    int result = 0;
    for (int i = 0; i < (argc - 2); ++i) {
        pthread_join(thread_ids[i], NULL);
        result += jobs[i].sum;
    }
    free(thread_ids);
    free(jobs);

    printf("%d lines matching \"%s\" found across %d files.\n", result, argv[1], argc - 2);

    return 0;
}

void *count_in_file(void *job) {
    Job *my_job = job;
    
    // open file to read
    FILE *my_file = fopen(my_job->file_path, "r");
    if (my_file == NULL) {
        perror(my_job->file_path);
        exit(EXIT_FAILURE);
    }

    // initialize counter of matching lines as 0
    my_job->sum = 0;
    char curr_line[MAX_LINE_LENGTH];
    while (fgets(curr_line, MAX_LINE_LENGTH, my_file) != NULL) {
        // remove trailing newline
        char *newline = strchr(curr_line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        // increment counter iff line matches search term
        if (strcmp(curr_line, my_job->line) == 0) {
            my_job->sum += 1;
        }
    }

    if (fclose(my_file)) {
        perror(my_job->file_path);
        exit(EXIT_FAILURE);
    }

    return NULL;
}