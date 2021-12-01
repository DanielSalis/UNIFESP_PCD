#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>

int request = 0;
int response = 0;
int SOMA = 0;

void critical_section(int t)
{
    int local = SOMA;
    sleep(rand() % 2);
    SOMA = local + 1;

    printf("Thread id:(%d) CURRENT SUM: %d\n", t, SOMA);
}

void execute_client(int t)
{
    while (1)
    {
        // while (response != t)
        //     request = t;

        critical_section(t);

        response = 0;
    }
}

void execute_server()
{
    while (1)
    {
        while (request == 0)
            ;
        response = request;

        while (response != 0)
            ;
        request = 0;
    }
}

void execute_threads(int number_of_threads)
{
    omp_set_num_threads(number_of_threads + 1);

#pragma omp parallel
    {
        int t = omp_get_thread_num();

        if (t == 0)
            execute_server();
        else
            execute_client(t);
    }
}

int set_number_of_threads()
{

    int number_of_threads = 0;

    while (1)
    {
        printf("Set number of threads: ");
        scanf("%d", &number_of_threads);

        if (number_of_threads < 1)
        {
            printf("Invalid number of threads\n");
        }
        else
        {
            break;
        }
    }

    return number_of_threads;
}

int main(int argc, char const *argv[])
{

    int number_of_threads = set_number_of_threads();

    execute_threads(number_of_threads);

    return 0;
}