#include <pthread.h>
#include <sched.h>
#include <semaphore.h> //to use semaphores
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 235977

char **words;
int word_count;
int palindrome_count = 0;
int semordnilap_count = 0;
// pthread_mutex_t mutex;
int W;

int store_words(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file\n");
        return -1;
    }
    char line[100];

    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
        // Word file ends with a newline
        if (strlen(line) == 0)
        {
            break;
        }

        words[word_count] = strdup(line);
        word_count++;
    }

    fclose(file);
    return word_count;
}

void *worker(void *arg)
{
    int thread_id = *(int *)arg;
    int local_palindrome_count = 0;
    int local_semiordnilap_count = 0;

    // Divide words according to thread
    int start = (word_count / W) * thread_id;
    int end = (thread_id == W - 1) ? word_count : (word_count / W) * (thread_id + 1);

    printf("%d, %d, %d\n", thread_id, start, end);
    for (int i = start; i < end; i++)
    {
        // Reverse string
        int len = strlen(words[i]);
        char *reversed = malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++)
        {
            *(reversed + j) = *(*(words + i) + len - j - 1);
        }
        *(reversed + len) = '\0';

        if (strcmp(words[i], reversed) == 0)
        {
            local_palindrome_count++;
            FILE *file = fopen("result.txt", "a");
            fprintf(file, "%s is a palindrome\n", words[i]);
            fclose(file);
            free(reversed);
        }
        else
        {
            int j = 0;
            while (j < word_count)
            {
                if (strcmp(words[j], reversed) == 0)
                {
                    local_semiordnilap_count++;
                    FILE *file = fopen("result.txt", "a");
                    fprintf(file, "%s is a semiordnilap\n", words[i]);
                    fclose(file);
                    free(reversed);
                    break;
                }
                j++;
            }
        }
    }
    // pthread_mutex_lock(&mutex);
    palindrome_count += local_palindrome_count;
    semordnilap_count += local_semiordnilap_count;
    // pthread_mutex_unlock(&mutex);

    printf("Thread %d found %d palindromes and %d semordnilaps.\n", thread_id, local_palindrome_count, local_semiordnilap_count);

    return NULL;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <dictionary_file> <num_threads>\n", argv[0]);
        return 1;
    }

    // Clear the result file
    FILE *file = fopen("result.txt", "w");
    fclose(file);

    W = atoi(argv[2]);
    words = malloc(MAX_WORDS * sizeof(char *));
    word_count = store_words(argv[1]);
    pthread_t threads[W];
    int thread_ids[W];
    // pthread_mutex_init(&mutex, NULL);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < W; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
    }

    for (int i = 0; i < W; i++)
    {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("Total palindromes: %d\n", palindrome_count);
    printf("Total semordnilaps: %d\n", semordnilap_count);
    printf("Execution time: %f seconds\n", elapsed_time);

    // pthread_mutex_destroy(&mutex);
    for (int i = 0; i < word_count; i++)
    {
        free(words[i]);
    }
    free(words);
    return 0;
}