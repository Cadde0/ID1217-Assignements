#include <pthread.h>
#include <sched.h>
#include <semaphore.h> //to use semaphores

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "findPal.h"

#define MAX_WORDS 235977

int store_words(const char *filename, char **words)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file\n");
        return -1;
    }
    int word_count = 0;
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

int is_pal(char *word, char **words)
{
    int len = strlen(word);
    char *reversed = malloc(len * sizeof(char));
    for (int i = 0; i < len; i++)
    {
        *(reversed + i) = *(word + len - i - 1);
    }

    while (/*Condition*/)
    {
        if (/*reversed word is in dict*/)
        {
            /* write word to result file */
            return 1; // Is a pal
        }
    }

    free(reversed);

    return 0;
}

int main(int argc, char const *argv[])
{

    const char *input_file = argv[1];
    // Allocate memory for the words
    char **words = malloc(MAX_WORDS * sizeof(char *));

    // Store the words in the array
    int word_count = store_words(input_file, words);

    // Print the words to verify
    for (int i = 0; i < word_count; i++)
    {
        printf("%s\n", words[i]);
    }
    printf("Word count: %d\n", word_count);

    // Free the allocated memory
    for (int i = 0; i < MAX_WORDS; i++)
    {
        free(words[i]);
    }
    free(words);
    return 0;
}