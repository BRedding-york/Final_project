

struct ListNode* listNodeConstructor(int value);
void listNodeDestructor(struct ListNode* node);
struct List* listConstructor();
void listDestructor(struct List* linkedList);
int listIsEmpty(struct List* linkedList);
int listSize(struct List* linkedList);
int areEqual(struct List* list1, struct List* list2);
int listAdd(struct List* linkedList, int entity);

// Sorting functions

void sortQuick (struct List* list);
/* sorts a list using quick sort */

void sortBubble(struct List* list);
/* This function will take a list and sort it*/

void sortInsertion(struct List* list);
/* sorts a list using insertion sort*/
