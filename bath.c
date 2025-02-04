#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

void *man(void *arg)
{
    int thread_id = *(int *)arg;
    printf("Hello from male thread %d\n", thread_id);
    return NULL;
}

void *woman(void *arg)
{
    int thread_id = *(int *)arg;
    printf("Hello from female thread %d\n", thread_id);
    return NULL;
}

int main(int argc, char *args[])
{
    if (argc != 2)
    {
        printf("Usage: %s <num_threads>\n", args[0]);
    };

    int W = atoi(args[1]);
    pthread_t threads[W];
    int thread_ids[W];

    for (int i = 0; i < W; i++)
    {
        thread_ids[i] = i;
        if (i % 2 == 0)
        {
            pthread_create(&threads[i], NULL, man, &thread_ids[i]);
        }
        else
        {
            pthread_create(&threads[i], NULL, woman, &thread_ids[i]);
        };
    }

    return 0;
}
