#include "lab2.h"
#include <cstring>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

#define NUMBER_OF_THREADS 11   // a,b,c,d,e,f,g,h,i,k,m
pthread_t tid[NUMBER_OF_THREADS];
pthread_mutex_t lock;
// 1
sem_t semA1, semC1, semD1, semE1, phase1_done;

// 2: bcde 
sem_t semB2_step, semC2_step, semD2_step, semE2_step, micro_bcde_done;
sem_t semB_rem,  semC_rem,  semD_rem,  semE_rem,  phase2_done;

// 3: degh
sem_t semD3, semE3, semGpre, semHpre, phase3_done;

// efgh
sem_t semE_step, semF_step, semG_step, semH_step, ring_done;

// 5> 6
sem_t semHtail, semIgo, semKgo, semMgo;

static const int M = 4;      // единый множитель 4 символа на интервал
static int err = 0;          // для проверок pthread_create

//прототипы потоков
void *thread_a(void *ptr);
void *thread_b(void *ptr);
void *thread_c(void *ptr);
void *thread_d(void *ptr);
void *thread_e(void *ptr);
void *thread_f(void *ptr);
void *thread_g(void *ptr);
void *thread_h(void *ptr);
void *thread_i(void *ptr);
void *thread_k(void *ptr);
void *thread_m(void *ptr);

// утилита печати одного символа (с защитой)
static inline void put_char(char ch)
{
    pthread_mutex_lock(&lock);
    std::cout << ch << std::flush;
    pthread_mutex_unlock(&lock);
    computation();
}

//aункции по заданию
unsigned int lab2_thread_graph_id() { return 4; }
const char*  lab2_unsynchronized_threads() { return "bcde"; }
const char*  lab2_sequential_threads()    { return "efgh"; }

//реализации потоков

void *thread_a(void *ptr)
{
    sem_wait(&semA1);                  // 1
    for (int i =0;i<M;++i) put_char('a');
    sem_post(&phase1_done);
    return ptr;
}

void *thread_b(void *ptr)
{
    //BBBBBB:
    sem_wait(&semB2_step);
    put_char('b');
    sem_post(&semC2_step);
    sem_wait(&semB_rem);
    for (int i=0; i<M-1;++i) put_char('b');
    sem_post(&phase2_done);
    return ptr;
}

void *thread_c(void *ptr)
{
    //CCCCCCCC
    sem_wait(&semC1);
    for (int i=0;i<M;++i) put_char('c');
    sem_post(&phase1_done);
    sem_wait(&semC2_step);
    put_char('c');
    sem_post(&semD2_step);
    sem_wait(&semC_rem);
    for (int i=0; i<M-1;++i) put_char('c');
    sem_post(&phase2_done);
    return ptr;
}

void *thread_d(void *ptr)
{
    // d
    sem_wait(&semD1);
    for (int i=0; i<M;++i) put_char('d');
    sem_post(&phase1_done);
    sem_wait(&semD2_step);
    put_char('d');
    sem_post(&semE2_step);
    sem_wait(&semD_rem);
    for (int i=0;i<M-1;++i) put_char('d');
    sem_post(&phase2_done);
    sem_wait(&semD3);
    for (int i=0;i<M;++i) put_char('d');
    sem_post(&phase3_done);
    return ptr;
}

void *thread_e(void *ptr)
{
    sem_wait(&semE1);
    for (int i=0;i<M;++i) put_char('e');
    sem_post(&phase1_done);
    sem_wait(&semE2_step);
    put_char('e');
    sem_post(&micro_bcde_done);
    sem_wait(&semE_rem);
    for (int i=0;i<M-1;++i) put_char('e');
    sem_post(&phase2_done);
    sem_wait(&semE3);
    for (int i=0; i<M;++i) put_char('e');
    sem_post(&phase3_done);
    for (int i=0;i< M;++i) {
        sem_wait(&semE_step);
        put_char('e');
        sem_post(&semF_step);
    }
    return ptr;
}

void *thread_f(void *ptr)
{
    for (int i =0;i<M; ++i) {
        sem_wait(&semF_step);
        put_char('f');
        sem_post(&semG_step);
    }
    return ptr;
}

void *thread_g(void *ptr)
{
    sem_wait(&semGpre);
    for (int i=0; i<M;++i) put_char('g');
    sem_post(&phase3_done);
    for (int i=0; i<M; ++i) {
        sem_wait(&semG_step);
        put_char('g');
        sem_post(&semH_step);
    }
    return ptr;
}

void *thread_h(void *ptr)
{
    sem_wait(&semHpre);
    for (int i = 0;i<M;++i) put_char('h');
    sem_post(&phase3_done);
    for (int i = 0;i<M;++i) {
        sem_wait(&semH_step);
        put_char('h');
        if (i < M - 1) sem_post(&semE_step);   // запуск следующего круга (кроме последнего)
    }
    sem_post(&ring_done);                       
    sem_wait(&semHtail);
    for (int i = 0;i<M;++i) put_char('h');
    return ptr;
}

void *thread_i(void *ptr)
{
    sem_wait(&semIgo);
    for (int i = 0;i<M;++i) put_char('i');
    sem_post(&semKgo); 
    return ptr;
}

void *thread_k(void *ptr)
{
    sem_wait(&semKgo);
    for (int i =0;i<M;++i) put_char('k');
    sem_post(&semMgo); 
    return ptr;
}

void *thread_m(void *ptr)
{
    sem_wait(&semMgo);
    for (int i =0;i<M;++i) put_char('m');
    return ptr;
}
int lab2_init()
{
    if (pthread_mutex_init(&lock, NULL) != 0) {
        std::cerr << "Mutex init failed" << std::endl;
        return 1;
    }
    auto S = [](sem_t &s) -> int { return sem_init(&s, 0, 0); };
    
    
    

    // init semaphores
    if ( S(semA1) || S(semC1) || S(semD1) || S(semE1) || S(phase1_done) ) {
        std::cerr << "Sem init phase1 failed" << std::endl; return 1;
    }
    if ( S(semB2_step) || S(semC2_step) || S(semD2_step) || S(semE2_step) || S(micro_bcde_done) ) {
        std::cerr << "Sem init micro-bcde failed" << std::endl; return 1;
    }
    if ( S(semB_rem) || S(semC_rem) || S(semD_rem) || S(semE_rem) || S(phase2_done) ) {
        std::cerr << "Sem init phase2 failed" << std::endl; return 1;
    }
    if ( S(semD3) || S(semE3) || S(semGpre) || S(semHpre) || S(phase3_done) ) {
        std::cerr << "Sem init phase3 failed" << std::endl; return 1;
    }
    if ( S(semE_step) || S(semF_step) || S(semG_step) || S(semH_step) || S(ring_done) ) {
        std::cerr << "Sem init ring failed" << std::endl; return 1;
    }
    if ( S(semHtail) || S(semIgo) || S(semKgo) || S(semMgo) ) {
        std::cerr << "Sem init tail failed" << std::endl; return 1;
    }
    err = pthread_create(&tid[0],  NULL, thread_a, NULL);
    if (err) std::cerr << "Can't create thread a. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[1],  NULL, thread_b, NULL);
    if (err) std::cerr << "Can't create thread b. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[2],  NULL, thread_c, NULL);
    if (err) std::cerr << "Can't create thread c. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[3],  NULL, thread_d, NULL);
    if (err) std::cerr << "Can't create thread d. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[4],  NULL, thread_e, NULL);
    if (err) std::cerr << "Can't create thread e. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[5],  NULL, thread_f, NULL);
    if (err) std::cerr << "Can't create thread f. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[6],  NULL, thread_g, NULL);
    if (err) std::cerr << "Can't create thread g. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[7],  NULL, thread_h, NULL);
    if (err) std::cerr << "Can't create thread h. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[8],  NULL, thread_i, NULL);
    if (err) std::cerr << "Can't create thread i. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[9],  NULL, thread_k, NULL);
    if (err) std::cerr << "Can't create thread k. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[10], NULL, thread_m, NULL);
    if (err) std::cerr << "Can't create thread m. Error: " << strerror(err) << std::endl;
    
    
    
    
    sem_post(&semA1);
    sem_post(&semC1);
    sem_post(&semD1);
    sem_post(&semE1);
    
    for (int i =0;i<4; ++i) sem_wait(&phase1_done);
    
    sem_post(&semB2_step);         
    sem_wait(&micro_bcde_done);
    sem_post(&semB_rem);
    sem_post(&semC_rem);
    sem_post(&semD_rem);
    sem_post(&semE_rem);
    
    for (int i=0;i<4; ++i) sem_wait(&phase2_done);

    sem_post(&semD3);
    sem_post(&semE3);
    sem_post(&semGpre);
    sem_post(&semHpre);
    
    for (int i =0; i<4;++i) sem_wait(&phase3_done);

    sem_post(&semE_step);
    sem_wait(&ring_done);            

    sem_post(&semHtail);
    sem_post(&semIgo);              

    for (int i = 0;i< NUMBER_OF_THREADS;++i) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    sem_destroy(&semA1); sem_destroy(&semC1); sem_destroy(&semD1); sem_destroy(&semE1); sem_destroy(&phase1_done);
    sem_destroy(&semB2_step); sem_destroy(&semC2_step); sem_destroy(&semD2_step); sem_destroy(&semE2_step); sem_destroy(&micro_bcde_done);
    sem_destroy(&semB_rem); sem_destroy(&semC_rem); sem_destroy(&semD_rem); sem_destroy(&semE_rem); sem_destroy(&phase2_done);
    sem_destroy(&semD3); sem_destroy(&semE3); sem_destroy(&semGpre); sem_destroy(&semHpre); sem_destroy(&phase3_done);
    sem_destroy(&semE_step); sem_destroy(&semF_step); sem_destroy(&semG_step); sem_destroy(&semH_step); sem_destroy(&ring_done);
    sem_destroy(&semHtail); sem_destroy(&semIgo); sem_destroy(&semKgo); sem_destroy(&semMgo);

    std::cout << std::endl;
    return 0;
}

