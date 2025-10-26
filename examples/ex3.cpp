#include <iostream>
#include <cstring> // need this for strerror
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

// compile with -lpthreads

#define NUMBER_OF_C_THREADS 1
#define NUMBER_OF_P_THREADS 1
#define NUMBER_OF_THREADS (NUMBER_OF_C_THREADS+NUMBER_OF_P_THREADS)

pthread_t tid[NUMBER_OF_THREADS];
pthread_mutex_t lock;  // critical section lock
sem_t *semA, *semB;


// consumer thread from the producer-consumer problem
void *thread_c(void *ptr)
{
    char const* thread_msg = (char*) ptr;
    for (int i = 0; i < 3; ++i) {
        sem_wait(semA);
        pthread_mutex_lock(&lock);
        std::cout << thread_msg << std::flush;
        pthread_mutex_unlock(&lock);
        sem_post(semB);
    }
    return  ptr;
}

// producer thread from the producer-consumer problem
void *thread_p(void *ptr)
{
    char const* thread_msg = (char*) ptr;
    for (int i = 0; i < 3; ++i) {
        sem_wait(semB);
        pthread_mutex_lock(&lock);
        std::cout << thread_msg << std::flush;
        pthread_mutex_unlock(&lock);
        sem_post(semA);
    }
    return ptr;
}


int main(int argc, char **argv)
{
    char const * thread_texts[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"};
    int err;
    // initilize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        std::cerr << "Mutex init failed" << std::endl;
        return 1;
    }
    //initialize semaphores
    if ( (semA = sem_open("/my_semaphore2", O_CREAT, 0777, 0)) == SEM_FAILED ) {
        std::cerr << "Semaphore #2 init failed" << std::endl;
        return 1;
    }
    if ( (semB = sem_open("/my_semaphore1", O_CREAT, 0777, 1)) == SEM_FAILED ) {
        std::cerr << "Semaphore #1 init failed" << std::endl;
        return 1;
    }

    // start the threads
    unsigned int thread_count = 0;
    for (int i = 0; i < NUMBER_OF_P_THREADS; ++i) {
        err = pthread_create(&tid[thread_count], NULL, thread_p, (void *) thread_texts[thread_count]);
        if (err != 0)
            std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
        else
            ++thread_count;
    }
    for (int i = 0; i < NUMBER_OF_C_THREADS; ++i) {
        err = pthread_create(&tid[thread_count], NULL, thread_c, (void *) thread_texts[thread_count]);
        if (err != 0)
            std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
        else
            ++thread_count;
    }

    // wait for threads to finish
    for (int i = 0; i < thread_count; ++i) {
        pthread_join(tid[i], NULL);
    }
    // destroy mutex
    pthread_mutex_destroy(&lock);
    // close semaphores
    sem_close(semA);
    sem_close(semB);
    // destroy semaphores
    sem_unlink("/my_semaphore1");
    sem_unlink("/my_semaphore2");
    // print a new line
    std::cout << std::endl;
    return 0;
}
