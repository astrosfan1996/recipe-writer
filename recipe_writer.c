/*
 * recipe_writer.c
 * version 1.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "recipe_writer.h"

void strip_newline_char(char* str);
void space_to_underscore(char* str);

int main()
{
    printf("Welcome to recipe writer!\n\n");
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
    char* name = malloc(128);
    do {
        fgets(name, 128, stdin);
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
        ingredient->data = malloc(128);
        fgets(ingredient->data, 128, stdin);
        if(ingredient->data[0] == '\n') {
            free(ingredient->data);
            free(ingredient);
            if(prev != NULL) {
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
        direction->data = malloc(1024);
        fgets(direction->data, 1024, stdin);
        if(direction->data[0] == '\n') {
            free(direction->data);
            free(direction);
            if(prev != NULL) {
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
        printf("Notes:\n");
        notes = malloc(sizeof(ListNode));
        ListNode* note = notes;
        prev = NULL;
        while(true) {
            printf("  - ");
            note->data = malloc(1024);
            fgets(note->data, 1024, stdin);
            if(note->data[0] == '\n') {
                free(note->data);
                free(note);
                if(prev != NULL) {
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

void space_to_underscore(char* str)
{
    int len = strlen(str);
    for(int i = 0; i < len; ++i) {
        if(str[i] == ' ') {
            str[i] = '_';
        }
    }
}

void write_recipe_to_file(Recipe* rec)
{
    char* filename = malloc(strlen(rec->name) + 4); // add chars for '.md' and terminating null
    sprintf(filename, "%s.md", rec->name);
    space_to_underscore(filename);
    printf("Writing recipe for %s to file '%s'\n", rec->name, filename);

    FILE* file = fopen(filename, "wx");
    if(errno == EEXIST) {
        do {
            printf("File %s already exists, please choose a different name: ", filename);
            free(filename);
            filename = malloc(128);
            do {
                fgets(filename, 128, stdin);
            } while(filename[0] == '\n');
            strip_newline_char(filename);
            errno = 0;
            file = fopen(filename, "wx");
        } while(errno == EEXIST);
    }
    free(filename);

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
    printf("Done writing %s recipe to file\n\n", rec->name);
}

bool run_again(char* question)
{
    printf("%s\n", question);
    char* ans = malloc(128);

    do {
        fgets(ans, 128, stdin);
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
    str[len - 1] = 0;
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