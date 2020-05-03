/*
 * recipe_writer.h
 */

#define ANSWER_BUFFER_MAX_SIZE 128
#define RECIPE_NAME_MAX_SIZE 128
#define INGREDIENT_NAME_MAX_SIZE 128
#define DIRECTION_TEXT_MAX_SIZE 1024
#define NOTE_TEXT_MAX_SIZE 1024
#define FILENAME_MAX_SIZE 64

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
bool run_again(const char* question);