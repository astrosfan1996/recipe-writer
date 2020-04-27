/*
 * recipe_writer.h
 * version 1.0
 */

typedef struct ListNode {
    char* data;
    struct ListNode* next;
} ListNode;

typedef struct {
    char* name;
    ListNode* ingredients;
    ListNode* directions;
    ListNode* notes;
} Recipe;

void free_recipe(Recipe* rec);
void free_list(ListNode* list);
Recipe* get_recipe();
void write_recipe_to_file(Recipe* rec);
bool run_again();