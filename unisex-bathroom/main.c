#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h> // For O_CREAT and O_EXCL

#define NUM_PEOPLE 10

sem_t *bathroom;
sem_t *queue_lock;
int men_in_bathroom = 0, women_in_bathroom = 0;
int waiting_men = 0, waiting_women = 0;

void *man(void *arg)
{
    int id = *(int *)arg;

    while (1)
    {
        sleep(rand() % 5 + 3);

        sem_wait(queue_lock);
        waiting_men++;
        printf("Man %d is waiting. Men in queue: %d, Women in queue: %d\n", id, waiting_men, waiting_women);
        while (women_in_bathroom > 0)
        {
            sem_post(queue_lock);
            sem_wait(bathroom);
            sem_wait(queue_lock);
        }
        waiting_men--;
        men_in_bathroom++;
        printf("Man %d entered the bathroom. Men inside: %d, Women inside: %d\n", id, men_in_bathroom, women_in_bathroom);
        sem_post(queue_lock);

        sleep(rand() % 3 + 1);

        sem_wait(queue_lock);
        men_in_bathroom--;
        printf("Man %d left the bathroom. Men inside: %d, Women inside: %d\n", id, men_in_bathroom, women_in_bathroom);
        if (men_in_bathroom == 0 && waiting_women > 0)
        {
            for (int i = 0; i < waiting_women; i++)
                sem_post(bathroom);
        }
        sem_post(queue_lock);
    }
    return NULL;
}

void *woman(void *arg)
{
    int id = *(int *)arg;

    while (1)
    {
        sleep(rand() % 5 + 3);

        sem_wait(queue_lock);
        waiting_women++;
        printf("Woman %d is waiting. Women in queue: %d, Men in queue: %d\n", id, waiting_women, waiting_men);
        while (men_in_bathroom > 0)
        {
            sem_post(queue_lock);
            sem_wait(bathroom);
            sem_wait(queue_lock);
        }
        waiting_women--;
        women_in_bathroom++;
        printf("Woman %d entered the bathroom. Women inside: %d, Men inside: %d\n", id, women_in_bathroom, men_in_bathroom);
        sem_post(queue_lock);

        sleep(rand() % 3 + 1);

        sem_wait(queue_lock);
        women_in_bathroom--;
        printf("Woman %d left the bathroom. Women inside: %d, Men inside: %d\n", id, women_in_bathroom, men_in_bathroom);
        if (women_in_bathroom == 0 && waiting_men > 0)
        {
            for (int i = 0; i < waiting_men; i++)
                sem_post(bathroom);
        }
        sem_post(queue_lock);
    }
    return NULL;
}

int main()
{
    pthread_t people[NUM_PEOPLE];
    bathroom = sem_open("/bathroom_sem", O_CREAT, 0644, 0);
    queue_lock = sem_open("/queue_lock_sem", O_CREAT, 0644, 1);

    for (int i = 0; i < NUM_PEOPLE; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        if (rand() % 2 == 0)
        {
            pthread_create(&people[i], NULL, man, id);
        }
        else
        {
            pthread_create(&people[i], NULL, woman, id);
        }
    }

    for (int i = 0; i < NUM_PEOPLE; i++)
    {
        pthread_join(people[i], NULL);
    }

    sem_close(bathroom);
    sem_close(queue_lock);
    sem_unlink("/bathroom_sem");
    sem_unlink("/queue_lock_sem");
    return 0;
}
