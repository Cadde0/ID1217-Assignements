#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h> // OpenMP header

#define MAX_WORDS 235977

char **words;
int word_count;
int palindrome_count = 0;
int semordnilap_count = 0;
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

void worker()
{
#pragma omp parallel
    {

        int local_palindrome_count = 0;
        int local_semiordnilap_count = 0;

        // Divide words according to thread
        int start = (word_count / W) * omp_get_thread_num();
        int end = (omp_get_thread_num() == W - 1) ? word_count : (word_count / W) * (omp_get_thread_num() + 1);

        printf("%d, %d, %d\n", omp_get_thread_num(), start, end);
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

        palindrome_count += local_palindrome_count;
        semordnilap_count += local_semiordnilap_count;

        printf("Thread %d found %d palindromes and %d semordnilaps.\n", omp_get_thread_num(), local_palindrome_count, local_semiordnilap_count);
    }
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
    omp_set_num_threads(W);

    double start_time = omp_get_wtime();
    worker();
    double end_time = omp_get_wtime();

    printf("Total palindromes: %d\n", palindrome_count);
    printf("Total semordnilaps: %d\n", semordnilap_count);
    printf("Execution time: %f seconds\n", (end_time - start_time));

    for (int i = 0; i < word_count; i++)
    {
        free(words[i]);
    }
    free(words);
    return 0;
}