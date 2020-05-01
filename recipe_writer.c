/*
 * recipe_writer.c
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "recipe_writer.h"
#include "version.h"

void strip_newline_char(char* str);
char* create_filename(const char* input);

int main()
{
    printf("Welcome to recipe writer!\nVersion %s\n\n", VERSION);
    do {
        Recipe* rec = get_recipe();
        write_recipe_to_file(rec);
        free_recipe(rec);
    } while(run_again("Would you like to add another recipe? [Y/n]"));
    printf("Goodbye!\n");
}

Recipe* get_recipe()
{
    printf("Creating new recipe\n");

    /* Name */
    printf("Name: ");
    char* name = malloc(RECIPE_NAME_MAX_SIZE);
    do {
        fgets(name, RECIPE_NAME_MAX_SIZE, stdin);
    } while(name[0] == '\n');
    strip_newline_char(name);
    printf("\n");

    /* Ingredients */
    printf("After entering the last ingredient, hit enter twice to finish with ingredients\n");
    printf("Ingredients:\n");
    ListNode* ingredients = malloc(sizeof(ListNode));
    ListNode* ingredient = ingredients;
    ListNode* prev = NULL;
    while(true) {
        printf("  - ");
        ingredient->data = malloc(INGREDIENT_NAME_MAX_SIZE);
        fgets(ingredient->data, INGREDIENT_NAME_MAX_SIZE, stdin);
        if(ingredient->data[0] == '\n') {
            free(ingredient->data);
            free(ingredient);
            ingredient = NULL;
            if(prev == NULL) {
                ingredients = NULL;
            } else {
                prev->next = NULL;
            }
            break;
        }
        strip_newline_char(ingredient->data);
        prev = ingredient;
        ingredient->next = malloc(sizeof(ListNode));
        ingredient = ingredient->next;
    }
    printf("\n");

    /* Directions */
    printf("After entering the last direction, hit enter twice to finish with directions\n");
    printf("Directions:\n");
    ListNode* directions = malloc(sizeof(ListNode));
    ListNode* direction = directions;
    prev = NULL;
    int i = 1;
    while(true) {
        printf("  %d. ", i++);
        direction->data = malloc(DIRECTION_TEXT_MAX_SIZE);
        fgets(direction->data, DIRECTION_TEXT_MAX_SIZE, stdin);
        if(direction->data[0] == '\n') {
            free(direction->data);
            free(direction);
            direction = NULL;
            if(prev == NULL) {
                directions = NULL;
            } else {
                prev->next = NULL;
            }
            break;
        }
        strip_newline_char(direction->data);
        prev = direction;
        direction->next = malloc(sizeof(ListNode));
        direction = direction->next;
    }
    printf("\n");

    /* Notes */
    ListNode* notes = NULL;

    if(run_again("Would you like to add any notes? [Y/n]")) {
        printf("After entering the last note, hit enter twice to finish with notes\n");
        printf("Notes:\n");
        notes = malloc(sizeof(ListNode));
        ListNode* note = notes;
        prev = NULL;
        while(true) {
            printf("  - ");
            note->data = malloc(NOTE_TEXT_MAX_SIZE);
            fgets(note->data, NOTE_TEXT_MAX_SIZE, stdin);
            if(note->data[0] == '\n') {
                free(note->data);
                free(note);
                note = NULL;
                if(prev == NULL) {
                    notes = NULL;
                } else {
                    prev->next = NULL;
                }
                break;
            }
            strip_newline_char(note->data);
            prev = note;
            note->next = malloc(sizeof(ListNode));
            note = note->next;
        }
    }
    printf("\n");

    Recipe* rec = malloc(sizeof(Recipe));
    rec->name = name;
    rec->ingredients = ingredients;
    rec->directions = directions;
    rec->notes = notes;

    return rec;
}

char* create_filename(const char* input)
{
    char* filename = malloc(FILENAME_MAX_SIZE);

    /* convert spaces to undercores */
    int len = strlen(input);
    int fpos = 0;
    for(int ipos = 0; ipos < len; ++ipos) {
        if(input[ipos] == ' ') {
            filename[fpos++] = '_';
        } else if(input[ipos] == '\'' || input[ipos] == ',') {
            continue;
        } else {
            filename[fpos++] = input[ipos];
        }
    }

    len = fpos;

    /* add '.md' file extension */
    int offset = len;
    int overflow = 0;
    if(len + 4 > FILENAME_MAX_SIZE) { // strlen() + '.md\0'
        overflow = len + 4 - FILENAME_MAX_SIZE;
        offset = len - overflow;
    }
    filename[offset] = '.';
    filename[offset + 1] = 'm';
    filename[offset + 2] = 'd';
    filename[offset + 3] = 0;

    return filename;
}

void write_recipe_to_file(Recipe* rec)
{
    char* filename = create_filename(rec->name);//malloc(strlen(rec->name) + 4); // add chars for '.md' and terminating null
    // sprintf(filename, "%s.md", rec->name);
    // create_filename(filename);
    printf("Writing recipe for \"%s\" to file \"%s\"\n", rec->name, filename);

    FILE* file = fopen(filename, "wx");
    if(errno == EEXIST) {
        do {
            printf("File \"%s\" already exists, please choose a different name: ", filename);
            free(filename);
            char* temp = malloc(FILENAME_MAX_SIZE);
            do {
                fgets(temp, FILENAME_MAX_SIZE, stdin);
            } while(temp[0] == '\n');
            strip_newline_char(temp);
            filename = create_filename(temp);
            free(temp);
            errno = 0;
            file = fopen(filename, "wx");
        } while(errno == EEXIST);
    }

    fprintf(file, "## %s\n\n", rec->name);

    fprintf(file, "### Ingredients\n");
    ListNode* ingredient = rec->ingredients;
    while(ingredient != NULL) {
        fprintf(file, "  - %s\n", ingredient->data);
        ingredient = ingredient->next;
    }
    fprintf(file, "\n");

    fprintf(file, "### Directions\n");
    int i = 1;
    ListNode* direction = rec->directions;
    while(direction != NULL) {
        fprintf(file, "  %d. %s\n", i++, direction->data);
        direction = direction->next;
    }

    if(rec->notes != NULL) {
        fprintf(file, "\n");
        fprintf(file, "### Notes\n");
        i = 0;
        ListNode* note = rec->notes;
        while(note != NULL) {
            fprintf(file, "  - %s\n", note->data);
            note = note->next;
        }
    }

    fclose(file);
    printf("Done writing \"%s\" recipe to file \"%s\"\n\n", rec->name, filename);
    free(filename);
}

bool run_again(char* question)
{
    printf("%s\n", question);
    char* ans = malloc(ANSWER_BUFFER_MAX_SIZE);

    do {
        fgets(ans, ANSWER_BUFFER_MAX_SIZE, stdin);
    } while(ans[0] == '\n');

    bool temp;
    switch(ans[0]) {
        case 'N':
        case 'n': {
            temp = false;
            break;
        }
        default: {
            temp = true;
            break;
        }
    }

    printf("\n");
    free(ans);
    return temp;
}

void strip_newline_char(char* str)
{
    int len = strlen(str);
    if(len > 0) {
        str[len - 1] = 0;
    }
}

void free_recipe(Recipe* rec)
{
    if(rec == NULL) {
        return;
    }

    free(rec->name);
    free_list(rec->ingredients);
    free_list(rec->directions);
    free_list(rec->notes);
    free(rec);
}

void free_list(ListNode* list)
{
    if(list == NULL) {
        return;
    }

    while(list != NULL) {
        ListNode* next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
}