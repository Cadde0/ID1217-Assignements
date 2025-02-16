#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h> // For O_CREAT and O_EXCL

#define NUM_PEOPLE 1000

sem_t *bathroom_lock;
sem_t *queue_lock;
int count = 0;
char current_gender = 'N';

typedef struct
{
    int id;
    char gender;
} Person;

typedef struct
{
    Person *people[NUM_PEOPLE];
    int front;
    int rear;
    int size;
} Queue;

Queue queue;

void init_queue()
{
    queue.front = 0;
    queue.rear = -1;
    queue.size = 0;
}

void enqueue(Person *person)
{
    if (queue.size < NUM_PEOPLE)
    {
        queue.rear = (queue.rear + 1) % NUM_PEOPLE;
        queue.people[queue.rear] = person;
        queue.size++;
    }
}

Person *dequeue()
{
    if (queue.size > 0)
    {
        Person *person = queue.people[queue.front];
        queue.front = (queue.front + 1) % NUM_PEOPLE;
        queue.size--;
        return person;
    }
    return NULL;
}

bool is_queue_empty()
{
    return queue.size == 0;
}

void *use_bathroom(void *arg)
{
    Person *person = (Person *)arg;
    while (1)
    {
        sleep(rand() % 3 + 1);

        sem_wait(queue_lock);
        enqueue(person);
        sem_post(queue_lock);

        while (1)
        {
            sem_wait(queue_lock);
            Person *next_person = queue.people[queue.front];
            if (next_person == person)
            {
                sem_wait(bathroom_lock);
                if (count == 0)
                {
                    current_gender = person->gender;
                }
                if (current_gender == person->gender)
                {
                    count++;
                    dequeue();
                    printf("%c %d entered the bathroom. Count: %d\n", person->gender, person->id, count);
                    sem_post(bathroom_lock);
                    sem_post(queue_lock);
                    break;
                }
                else
                {
                    sem_post(bathroom_lock);
                    sem_post(queue_lock);
                }
            }
            else
            {
                sem_post(queue_lock);
            }
        }

        sleep(rand() % 2 + 1);

        sem_wait(bathroom_lock);
        count--;
        printf("%c %d left the bathroom. Count: %d\n", person->gender, person->id, count);
        if (count == 0)
        {
            current_gender = (current_gender == 'M') ? 'F' : 'M';
        }
        sem_post(bathroom_lock);
    }
    return NULL;
}

int main()
{
    pthread_t people[NUM_PEOPLE];
    Person persons[NUM_PEOPLE];

    bathroom_lock = sem_open("/bathroom_lock_sem", O_CREAT, 0644, NUM_PEOPLE);
    queue_lock = sem_open("/queue_lock_sem", O_CREAT, 0644, 1);

    init_queue();

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

    sem_close(bathroom_lock);
    sem_close(queue_lock);
    sem_unlink("/bathroom_lock_sem");
    sem_unlink("/queue_lock_sem");

    return 0;
}