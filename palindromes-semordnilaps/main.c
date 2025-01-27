#include <pthread.h>
#include <sched.h>
#include <semaphore.h> //to use semaphores

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int i = 0;
    while (i < MAX_WORDS)
    {
        if (strcmp(words[i], reversed) == 0)
        {
            /* write word to result file */
            FILE *file = fopen("result.txt", "a");
            fprintf(file, "%s\n", word);
            fclose(file);
            return 1; // Is a pal
        }

        i++;
    }

    free(reversed);

    return 0;
}

int main(int argc, char const *argv[])
{

    const char *input_file = argv[1];
    // Clear the result file
    FILE *file = fopen("result.txt", "w");
    fclose(file);
    // Allocate memory for the words
    char **words = malloc(MAX_WORDS * sizeof(char *));

    // Store the words in the array
    int word_count = store_words(input_file, words);

    // Check if the words are palindromes
    for (int i = 0; i < word_count; i++)
    {
        if (is_pal(words[i], words) == 1)
        {
            printf("%s is a palindrome\n", words[i]);
        }
    }

    // Print the words to verify
    /*
    for (int i = 0; i < word_count; i++)
    {
        printf("%s\n", words[i]);
    }
    printf("Word count: %d\n", word_count);
    */

    // Free the allocated memory
    for (int i = 0; i < MAX_WORDS; i++)
    {
        free(words[i]);
    }
    free(words);
    return 0;
}