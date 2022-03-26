/*
 * Name: Sam Hoogewind (sth6)
 * Date: March 10, 2022
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_VIALS 30
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4

/* global variables */
sem_t register_sem, full, empty, client_sem, nurse_sem;
pthread_mutex_t mutex;
int registers_free = NUM_REGISTRATIONS_SIMULTANEOUSLY;
int num_vials_left = NUM_VIALS;
int in = 0;
int out = 0;
int buffer[NUM_STATIONS];

int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

// lower and upper are in seconds.
void walk(int lower, int upper) {
    usleep(get_rand_in_range(lower * 1000000, upper * 1000000)); // get a random number, converting the bounds to microseconds
}

// arg is the nurses station number.
void *nurse(void *arg) {
    long int id = (long int)arg;

    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id); // start the nurses shift

    while (num_vials_left > 0) { // while you still have vials left
        fprintf(stderr, "%s: nurse %ld walking to get a vial\n", curr_time_s(), id); // walk to the vials
        walk(1, 3); // wait 1-3 seconds to walk there
        if (num_vials_left - 1 < 0 ) { // check if you are 0 vials (dodge race conditions)
            break;
        } else {
            pthread_mutex_lock(&mutex); // take the lock to grab a vial
            num_vials_left --; // decrement the number of vials left
            pthread_mutex_unlock(&mutex); // release the lock
        }
        fprintf(stderr, "%s: nurse %ld got a vial. Number of vials left = %d\n", curr_time_s(), id, num_vials_left); // get a vial
        walk(1, 3); // wait 1-3 seconds to walk back
        fprintf(stderr, "%s: nurse %ld back at station\n", curr_time_s(), id); // the nurse gets back to their station

        for (int i = 0; i < SHOTS_PER_VIAL; i++) { // for each of the 6 shots
            fprintf(stderr, "%s: nurse %ld tells the waiting queue that they are available\n", curr_time_s(), id); // tell the queue you're ready
            fprintf(stderr, "%s: nurse %ld waiting for a client to arrive\n", curr_time_s(), id); // wait for the client to be ready

            sem_wait(&empty); // wait for the empty semaphore when there are no spots left
            pthread_mutex_lock(&mutex); // take the mutex lock
            buffer[in] = id; // insert your station into buffer
            in = (in + 1) % NUM_STATIONS; // increment what spot in the buffer you're at
            pthread_mutex_unlock(&mutex);  // unlock the mutex
            sem_post(&full); // signal the client that there is a station ready

            sem_wait(&client_sem); // wait for the client to indicate they are ready
            fprintf(stderr, "%s: nurse %ld sees client is ready. Giving shot now.\n", curr_time_s(), id); // acknowledge the client is ready
            walk(5, 5); // wait 5 seconds to give the client the shot
            fprintf(stderr, "%s: nurse %ld gave client the shot\n", curr_time_s(), id); // say you gave the shot
            sem_post(&nurse_sem); // let the client know you're done
            sem_post(&client_sem); // release the client

        }
    }

    fprintf(stderr, "%s: nurse %ld walking to get a vial\n", curr_time_s(), id); // walk back to get a vial
    walk(1, 3); // wait 1-3 seconds to walk there
    fprintf(stderr, "%s: nurse %ld found all vials are gone: exiting\n", curr_time_s(), id); // we know there are none left since we are out of the while loop
    fprintf(stderr, "%s: nurse %ld is done\n", curr_time_s(), id); // the nurse is finished

    pthread_exit(NULL); // exit the thread
}

void *client(void *arg) {
    long int id = (long int)arg;
    bool registered = false; // at the start they aren't registered

    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n",
            curr_time_s(), id);
    walk(3, 10); // walk 3-10 seconds to get to the desk
    fprintf(stderr, "%s: client %ld is waiting to register\n", curr_time_s(), id); // the client waits to register
    
    // stand in line
    while (registered == false) // while they aren't registered
    {
        if (registers_free > 0) // if there is a free registration desk
        {
            sem_wait(&register_sem); // take one of the register semaphores
            registers_free -= 1; // decrement how many registers are free
            fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id); // the client registers
            walk(3, 10); // walk 3-10 seconds to register
            registered = true; // set registered to true
            fprintf(stderr, "%s: client %ld is done registering. Now walking to next queue.\n", curr_time_s(), id); // the client is done registering
            registers_free += 1; // increment how many registers are free
            sem_post(&register_sem); // release the register semaphore
        }
    }

    walk(3, 10); // walk 3-10 seconds to get to the station assignment queue
    sem_wait(&full); // wait when there are no available stations
    pthread_mutex_lock(&mutex); // take the mutex lock
    int station = buffer[out]; // remove station from buffer
    fprintf(stderr, "%s: client %ld got assigned to station %d\n", curr_time_s(), id, station); // the client gets assigned a station
    out = (out + 1) % NUM_STATIONS; // increment where in the buffer you are
    walk(1, 2); // walk 1-2 seconds to get to the assigned station
    fprintf(stderr, "%s: client %ld is ready for the shot from nurse %d\n", curr_time_s(), id, station); // indicate the client is ready
    pthread_mutex_unlock(&mutex); // unlock the mutex lock
    sem_post(&empty); // release the semaphore

    sem_post(&client_sem); // signal to the nurse you're ready

    sem_wait(&nurse_sem); // wait for the nurse to signal they are done with the shot
    fprintf(stderr, "%s: client %ld got the shot!\n", curr_time_s(), id); // the client got the shot
    sem_post(&nurse_sem); // release the nurse

    fprintf(stderr, "%s: client %ld leaves the clinic!\n", curr_time_s(), id); // the client leaves the clinic

    pthread_exit(NULL);
}

int main() {
    // initialize the threads
    pthread_t nursethread[NUM_NURSES], clientthread[NUM_CLIENTS];
    
    // initialize semaphores
    sem_init(&register_sem, 0, NUM_REGISTRATIONS_SIMULTANEOUSLY);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, NUM_STATIONS);
    sem_init(&nurse_sem, 0, 0);
    sem_init(&client_sem, 0, 0);
    
    srand(time(0));

    // create all the nurse threads
    for (int i = 0; i < NUM_NURSES; i++)
    {
        pthread_create(&nursethread[i], NULL, nurse, (void *) (intptr_t) i);
    }

    // create all the client threads
    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        pthread_create(&clientthread[i], NULL, client, (void *) (intptr_t) i);
        walk(0, 1); // wait between 0-1 seconds between each thread
    }


    // join all the client threads
    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        pthread_join(clientthread[i], NULL);
    }

    // join all the nurse threads
    for (int i = 0; i < NUM_NURSES; i++)
    {
        pthread_join(nursethread[i], NULL);
    }

    // deinitialize all the semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&register_sem);
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&nurse_sem);
    sem_destroy(&client_sem);

    pthread_exit(0);
}
