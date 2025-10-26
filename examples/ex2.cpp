#include <iostream>
#include <cstring> // need this for strlen and strerror
#include <pthread.h>

// compile with -lpthreads

pthread_t tid[3];
pthread_mutex_t lock;
const int sleep_time = 10;

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}


void *thread1(void *ptr) // create the function to be executed as a thread
{
    char const* thread_msg = (char*) ptr;
    for (int i=0; i < strlen(thread_msg); ++i) {
        std::cout << thread_msg[i] << std::flush;
        sleep_ms(sleep_time);
    }
    return  ptr;
}

void *thread2(void *ptr) // create another function to be executed as a thread
{
    char const* thread_msg = (char*) ptr;
    pthread_mutex_lock(&lock);
    for (int i=0; i < strlen(thread_msg); ++i) {
        std::cout << thread_msg[i] << std::flush;
        sleep_ms(sleep_time);
    }
    pthread_mutex_unlock(&lock);
    return  ptr;
}

int main(int argc, char **argv)
{
    char const * thread_texts[] = {"Hello ", "world ", "program"};
    int err;
    // initilize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        std::cout << "Mutex init failed" << std::endl;
        return 1;
    }
    // Run three threads for the first time without protecting the critical section
    std::cout << "Running threads without a mutex: ";
    // start the threads
    for (int i = 0; i < 3; ++i) {
        err = pthread_create(&tid[i], NULL, thread1, (void *) thread_texts[i]);
        if (err != 0)
            std::cerr << "Can't create thread: " << strerror(err) << std::endl;
    }
    // wait for threads to finish
    for (int i = 0; i < 3; ++i) {
        pthread_join(tid[i], NULL);
    }
    
    // Run the threads for the second time, now using a mutex to protect critical section
    std::cout << std::endl << "Now running them with a mutex: ";
    // start the threads
    for (int i = 0; i < 3; ++i) {
        err = pthread_create(&tid[i], NULL, thread2, (void *) thread_texts[i]);
        if (err != 0)
            std::cerr << "Can't create thread: " << strerror(err) << std::endl;
    }
    // wait for threads to finish
    for (int i = 0; i < 3; ++i) {
        pthread_join(tid[i], NULL);
    }
    std::cout << std::endl;
    // destroy mutex
    pthread_mutex_destroy(&lock);
    return 0;
}
