/**
 * File: simple_threading.c
 * Description: a simple program that (crudely) emulates a threaded file server.
 *
 * Date: 2013-02-07
 *
 * Author: Jason Tierney
 */
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void * do_processing(void * arg);
void sig_handler(int);

char text[20];
float avg_time = 0;
int total_files = 0;
int total_time = 0;

/**
 * Main entry point.
 */
int main()
{
    pthread_t thread1;
    int status;
    
    signal(SIGINT, sig_handler);
    
    // Loop until the user uses CTRL+C to escape.
    while(1)
    {
        fflush(stdout); // Flush out stdout
        if(fgets(text, sizeof text, stdin) != NULL) // Query user input.
        {
            if((status = pthread_create(&thread1, NULL, do_processing, text)) != 0)
            {
                fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
		        exit(1);
            }
        }
    }

    sleep(3); // WTF is this doing here?
    return 0;
}

/**
 * Handles file processing.
 * Each thread will use this.
 * Simulates: 
 *   - 20% chance file will take 7-10 seconds to load.
 *   - 80% change file will take 1 second to load.
 */
void * do_processing(void* arg)
{
    // Our mutex lock. Prevents race conditions.
    pthread_mutex_t lock;

    // Get a percent between 1 - 100.
    int percent = rand() % 100 + 1;
    int sleep_time = 1;
    if(percent < 20) // 20% chance process will take 7-10 seconds.
    {
        sleep_time = rand() % 3 + 7;
    }

    sleep(sleep_time);
    printf("\nFile grabbed: %s It took: %i seconds.\n", arg, sleep_time);
    
    // Because threads "share" their data, we want to lock the following
    // code out from other threads. This is done using mutexes (mutexi? :D).
    // Otherwise, race conditions could happend.
    pthread_mutex_init(&lock, NULL); // Using our mutex to lock code.
    total_files++;
    total_time += sleep_time;
    pthread_mutex_destroy(&lock); // Destroys our lock.

    return arg;
}

/**
 * Handler for our awesome signal.
 */
void sig_handler(int signum)
{
    printf("\nPeace out, brah!");
    printf("\nTotal files: %i", total_files);
    avg_time = (float)total_time / (float)total_files;
    printf("\nAverage time: %.2f second(s)\n", avg_time);i

    // Exit gracefully.
    exit(0);
}
