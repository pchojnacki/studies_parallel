#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <omp.h>
#include <stdlib.h>
#include <malloc.h>

long long num_steps = 10000000000;
double step;

int main(int argc, char* argv[])
{
    clock_t start, stop;
    double pi, sum=0.0;
    long i;
    step = 1./(double)num_steps;
    start = clock();
    omp_set_num_threads(4);
    volatile double *suml;
    posix_memalign((void**)&suml, 64, 4*64);


#pragma omp parallel
    {
        cpu_set_t cpu;
        CPU_ZERO(&cpu);
        CPU_SET(omp_get_thread_num(), &cpu);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
        printf("%d \n", sched_getcpu());
        double x;
        int nCpu =  omp_get_thread_num() * 64;
        suml[nCpu] = 0;
#pragma omp for
        for (i=0; i<num_steps; i++)
        {
            x = (i + .5)*step;
            suml[nCpu] = suml[nCpu] + 4.0/(1.+ x*x);
        }
#pragma omp atomic
        sum +=  suml[nCpu];

    }
//    sum = suml[0] + suml[1];

    pi = sum*step;
    stop = clock();

    printf("Wartosc liczby PI wynosi %15.12f\n",pi);
    printf("Czas przetwarzania wynosi %f sekund\n",((double)(stop - start)/1000000.0));
    return 0;
}
