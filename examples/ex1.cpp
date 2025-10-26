#include <iostream>
#include <pthread.h>
#include <stdint.h> // need this for intptr_t type

// compile with -lpthreads


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


// create the function to be executed as a thread
void *thread(void *ptr)
{
    int thread_name = (intptr_t) ptr;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Thread " << thread_name << std::endl;
        sleep_ms(50);
    }
    return  ptr;
}

int main(int argc, char **argv)
{
    // create the thread objs
    pthread_t thread1, thread2, thread3;
    int thr1 = 1;
    int thr2 = 2;
    int thr3 = 3;
    // start the threads
    pthread_create(&thread1, NULL, thread, (void *) thr1);
    pthread_create(&thread2, NULL, thread, (void *) thr2);
    pthread_create(&thread3, NULL, thread, (void *) thr3);
    // wait for threads to finish
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
    return 0;
}
