#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h> // For O_CREAT and O_EXCL

#define NUM_PEOPLE 1000
#define MAX_CAPACITY 3 // Maximum number of people in the bathroom at the same time

sem_t *bathroom_lock;
sem_t *queue_lock;
sem_t *male_queue;
sem_t *female_queue;
int count = 0;
char current_gender = 'N';

typedef struct
{
    int id;
    char gender;
} Person;

void *use_bathroom(void *arg)
{
    Person *person = (Person *)arg;
    while (1)
    {
        sleep(rand() % 3 + 1);

        if (person->gender == 'M')
        {
            sem_wait(male_queue);
        }
        else
        {
            sem_wait(female_queue);
        }

        sem_wait(queue_lock);
        if (count == 0)
        {
            current_gender = person->gender;
        }
        if (current_gender == person->gender && count < MAX_CAPACITY)
        {
            count++;
            printf("%c %d entered the bathroom. Count: %d\n", person->gender, person->id, count);
            sem_post(queue_lock);
        }
        else
        {
            sem_post(queue_lock);
            if (person->gender == 'M')
            {
                sem_post(male_queue);
            }
            else
            {
                sem_post(female_queue);
            }
            continue;
        }

        sleep(rand() % 2 + 1);

        sem_wait(queue_lock);
        count--;
        printf("%c %d left the bathroom. Count: %d\n", person->gender, person->id, count);
        if (count == 0)
        {
            current_gender = 'N';
        }
        sem_post(queue_lock);

        if (person->gender == 'M')
        {
            sem_post(male_queue);
        }
        else
        {
            sem_post(female_queue);
        }
    }
    return NULL;
}

int main()
{
    pthread_t people[NUM_PEOPLE];
    Person persons[NUM_PEOPLE];

    bathroom_lock = sem_open("/bathroom_lock_sem", O_CREAT, 0644, MAX_CAPACITY);
    if (bathroom_lock == SEM_FAILED)
    {
        perror("sem_open/bathroom_lock");
        exit(EXIT_FAILURE);
    }
    queue_lock = sem_open("/queue_lock_sem", O_CREAT, 0644, 1);
    if (queue_lock == SEM_FAILED)
    {
        perror("sem_open/queue_lock");
        exit(EXIT_FAILURE);
    }
    male_queue = sem_open("/male_queue_sem", O_CREAT, 0644, 1);
    if (male_queue == SEM_FAILED)
    {
        perror("sem_open/male_queue");
        exit(EXIT_FAILURE);
    }
    female_queue = sem_open("/female_queue_sem", O_CREAT, 0644, 1);
    if (female_queue == SEM_FAILED)
    {
        perror("sem_open/female_queue");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_PEOPLE; i++)
    {
        persons[i].id = i;
        persons[i].gender = (i % 2) ? 'M' : 'F';
        pthread_create(&people[i], NULL, use_bathroom, &persons[i]);
    }

    for (int i = 0; i < NUM_PEOPLE; i++)
    {
        pthread_join(people[i], NULL);
    }

    sem_unlink("/bathroom_lock_sem"); // Unlink before closing
    sem_unlink("/queue_lock_sem");    // Unlink before closing
    sem_unlink("/male_queue_sem");    // Unlink before closing
    sem_unlink("/female_queue_sem");  // Unlink before closing
    sem_close(bathroom_lock);
    sem_close(queue_lock);
    sem_close(male_queue);
    sem_close(female_queue);

    return 0;
}