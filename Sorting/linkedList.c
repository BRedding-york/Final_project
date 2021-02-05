#include <stdio.h>
#include <stdlib.h>

#include "list.h"

// Declare hidden functions
void partition(struct List* linkedList, struct ListNode* low, struct ListNode* high);
void quickSort(struct List* linkedList, struct ListNode* low, struct ListNode* high, struct ListNode* prevHigh);

// struct def
struct ListNode
{
    int data;

    struct ListNode* next;
    struct ListNode* prev;
};

struct List
{
    int size;
    int isSorted;

    struct ListNode* head;
    struct ListNode* tail;
};
//end struct def


struct ListNode* listNodeConstructor(int value){
    struct ListNode* newNode;

    newNode = malloc(sizeof(struct ListNode));
    newNode->data = value;
    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;
}


void listNodeDestructor(struct ListNode* node)
{
    free(node);
}

struct List* listConstructor()
{
    struct List* newLinkedList;

    newLinkedList = malloc(sizeof(struct List));

    newLinkedList->size = 0;
    newLinkedList->head = NULL;
    newLinkedList->tail = NULL;

    return newLinkedList;
}

void listDestructor(struct List* linkedList)
{
    struct ListNode* currentRecord;
    struct ListNode* nextRecord = linkedList->head;

    while(nextRecord != NULL)
    {
        currentRecord = nextRecord;
        nextRecord = currentRecord->next;

        listNodeDestructor(currentRecord);
    }

    free(linkedList);
}

int listSize(struct List* linkedList)
/* Checks size of list*/
{
    int size = 0;
    struct ListNode* currentRecord;
    struct ListNode* nextRecord = linkedList->head;

    while (nextRecord != NULL)
    {
        size ++;
        currentRecord = nextRecord;
        nextRecord = currentRecord->next;
    }
    return size;
}

int listIsEmpty(struct List* linkedList)
/* Checks if a list is empty*/
{
    if (linkedList->size != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int listAdd(struct List* linkedList, int entity)
/* Add an entity to a list */
{
    struct ListNode* node;

    if (listIsEmpty(linkedList))
    {
        node = listNodeConstructor(entity);
        linkedList->head = node;
        linkedList->tail = node;
        linkedList->size ++;
    }
    else
    {
        node = listNodeConstructor(entity);
        linkedList->tail->next = node;
        node->prev = linkedList->tail;
        linkedList->tail = node;
        linkedList->size ++;
    }

    if (linkedList->size == listSize(linkedList))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void listDisplay (struct List* linkedList)
/* Displays a list*/
{
    struct ListNode* currentRecord;
    struct ListNode* nextRecord = linkedList->head;
    int place = 1;

    while(place != linkedList->size)
    {
        currentRecord = nextRecord;
        nextRecord = currentRecord->next;
        printf("%d, ", currentRecord->data);
        place ++;
    }
    printf("%d.\n", nextRecord->data);
}

int areEqual(struct List* list1, struct List* list2){
    /*will compare two lists to see if they are equal*/
    int total = 0;
    struct ListNode* listNode1 = list1->head;
    struct ListNode* listNode2 = list2->head;
    while(total + listNode1){
        total = 0;
        total = (listNode1->data == listNode2->data);
        listNode1 = listNode1->next;
        listNode2 = listNode2->next;
    }
    return !total;
}

// Sorting algorithms

/** This function will sort a list using insertion sort.
 * @param linkedList,   The linked list to be sorted.
 */
void sortInsertion(struct List* linkedList){
    struct ListNode* node;
    struct ListNode* nextNode;
    struct ListNode* walker;
    nextNode = linkedList->head->next;

    do{
        node = nextNode;
        walker = linkedList->head;
        if(walker->data > node->data){
            if(walker->prev){// not the head
                nextNode = node->next;
                node->prev = walker->prev;
                node->next = walker;
                walker->prev->next = node;
                walker->prev = node;
            }else{// the head
                nextNode = node->next;
                node->next = walker;
                node->prev = NULL;
                walker->prev = node;
                linkedList->head = node;
            }
        }else{
            if(!walker->next){//the tail
                nextNode = node->next;
                node->next = NULL;
                node->prev = walker;
                walker->next = node;
                linkedList->tail = node;
            }else{
                walker = walker->next;
            }
        }
    }while(nextNode);
    linkedList->isSorted = 1;
}

/** This function sorts a list using bubble sort.
 * @param linkedList,   The linked list to be sorted.
 */
void sortBubble(struct List* linkedList){
    struct ListNode* temp = malloc(sizeof(struct ListNode));
    struct ListNode* node = linkedList->head;
    struct List listCopy;

    do{
        listCopy = *linkedList;
        while (node){
            if(node->next->data < node->data){
                if(node->next->next){
                    node->prev->next = node->next;
                    temp->next = node->next->next;
                    node->next->next = node;
                    node->next->prev = node->prev;
                    node->prev = node->next;
                    node->next = temp->next;
                    node->next->prev = node;
                }
            }
            node = node->next;
        }
    }while(areEqual(linkedList, &listCopy));
    linkedList->isSorted = 1;
}

/** This function will sort a list using quick sort.
 * @param linkedList,   The linked list to be sorted.
 */
void sortQuick(struct List* linkedList){
    struct ListNode* low = linkedList->head;
    struct ListNode* high = linkedList->tail;
    high->prev = NULL;
    quickSort(linkedList, low, high, NULL);
    linkedList->isSorted = 1;
}


// Hidden functions

/** This function is the recursive part of the quick sort algorithm
 * @param linkedList,   The linked list being sorted.
 * @param low,          The low limit of the section of the list being sorted
 * @param high,         The high limit of the section of the list being sorted
 */
void quickSort(struct List* linkedList, struct ListNode* low, struct ListNode* high, struct ListNode* prevHigh)
{
    printf("new \n");
    /*if (high->data > low->data)
    {*/
        if(high){
            partition (linkedList, low, high);
            if(high->prev){
            printf("here 1");
                quickSort(linkedList, low, high->prev, high);
            }
            if(prevHigh){
            printf("here 2");
                quickSort(linkedList, high, prevHigh->prev, prevHigh);
            }else{
            printf("here 3");
            //printf("%p, %p, %p \n", high, linkedList->tail, prevHigh);
                quickSort(linkedList, high, linkedList->tail, prevHigh);
            }
        }
    //}
}

/**
 * @param linkedList,   The linked list being sorted.
 * @param low,          The low limit of the section of the list being sorted
 * @param high,         The high limit of the section of the list being sorted
 */
void partition(struct List* linkedList, struct ListNode* low, struct ListNode* high)
{
    struct ListNode* walker;
    struct ListNode* nextWalker;
    struct ListNode* temp;
    if(low == NULL){
        walker = linkedList->head;
    }else{
        walker = low->next;
    }
    printf("current %d, %p, next-%p, prev-%p\n", high->data, high, high->next, high->prev);
    while(walker != high){
        nextWalker = walker->next;
        //printf("walker %d, prev-%p, next-%p, address-%p\n", walker->data, walker->prev, walker->next, walker);
        if(walker->data < high->data){//less than
            printf("lower \n");
            if(high->prev == NULL){
                linkedList->head = walker;
            }else{
                high->prev->next = walker;
            }
            walker->prev = high->prev;
            walker->next = high;
            high->prev = walker;
        }else{//greater than or equal to high
            printf("higher \n");
            if(high->next == NULL){
                linkedList->tail = walker;
            }
            walker->next = high->next;
            walker->prev = high;
            if(walker->next){
                printf("here");
                walker->next->prev = walker;
            }
            high->next = walker;
        }
        printf("walker %d, prev-%p, next-%p, address-%p\n", walker->data, walker->prev, walker->next, walker);
        walker = nextWalker;
    }
    if(!high->next){
            printf("hacked");
        linkedList->tail = high;
    }
    //printf("prev? %p, next %p, address %p\n", linkedList->head->prev, linkedList->head->next, linkedList->head);
    if(!high->prev){
        printf("hacked");
        linkedList->head = high;
    }
    printf("head data %d, tail data %d\n", linkedList->head->data, linkedList->tail->data);
    walker = linkedList->head;
    while(walker){
        printf("%d ", walker->data);
        walker = walker->next;
    }
    printf("end");
}
