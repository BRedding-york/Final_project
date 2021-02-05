#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/**/

// Define constants
#define ARRAY_LIST_CAPACITY 50

// Declare hidden functions
void sortSwap(struct List* list, int index_1, int index_2);
int partition(struct List* list, int low, int high);
void quickSort(struct List* list, int low, int high);
int checkListLength(struct List* list);

//struct def
struct List
/* List structure */
{
    int capacity;
    int numEntities;
    int isSorted;
    int* entities;
};
//end struct def

struct List* listConstructor()
/* Dynamically creates a list*/
{
    struct List* arrayList;

    arrayList = malloc(sizeof(struct List));
    arrayList->entities = malloc(ARRAY_LIST_CAPACITY * sizeof(int));

    arrayList->capacity = ARRAY_LIST_CAPACITY;
    arrayList->numEntities = 0;
    arrayList->isSorted = 1;

    return arrayList;
}

void listDestructor(struct List* list)
/* Destroys a list */
{
    free(list->entities);
    free(list);
}

int listAdd(struct List* list, int entity)
/* Add an entity to a list */
{
    if (checkListLength(list) == 1)
    {
        printf("List full.\n");
        list->capacity = list->capacity*2;
        printf("Increasing list to twice capacity\n");
        list->entities = realloc(list->entities, list->capacity* sizeof(int));
        return 1;
    }
    else
    {
        list->entities[list->numEntities] = entity;
        list->numEntities ++;
    }
    list->isSorted = 0;

    return 0;
}

int checkListLength(struct List* list)
/* Checks if a list is full*/
{
    if (list->numEntities == list->capacity)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void listDisplay (struct List* list)
/* Displays a list*/
{
    int num = 0;
    while (num < list->numEntities-1)
    {
        printf("%d, ", list->entities[num]);
        num ++;
    }
    printf("%d.\n", list->entities[num]);
}


// Sorting algorithms

/** This function will sort a list using insertion sort
 * @param list, The array list pointer to be sorted
 */
void sortInsertion(struct List* list)
{
    int temp;
    int added;

    for (int i = 0; i < list -> numEntities; i++){
        temp = list->entities[i];
        added = 0;
        for (int j = i-1; j >= 0; j--){
            if (added == 0){
                if (temp > list->entities[j]){
                    list->entities[j+1] = list->entities[j];
                }
                else{
                    list->entities[j] = temp;
                    added = 1;
                }
            }
        }
    }
    list->isSorted = 1;
}

/** This function will sort a list using bubble sort.
 * @param list, The array list to be sorted.
 */
void sortBubble(struct List* list)
{
    int sorted = 0;
    int i;

    while (sorted == 0)
    {
        sorted = 1;
        for (i = 1 ; i < list->numEntities; i++)
        {
            if(list->entities[i-1] > list->entities[i])
            {
                sorted = 0;
                sortSwap(list, i, i-1);
            }
        }
    }
    list->isSorted = 1;
}

/** This function will sort a list using quick sort.
 * @param list, The array list to be sorted.
 */
void sortQuick (struct List* list)
{
    int low = 0;
    int high = list->numEntities - 1;
    quickSort(list, low, high);
    list->isSorted = 1;
}


// Periphery functions used by sorting functions

/** This function will swap two index's in a list.
 * @param list,     The list containing the two values to be swapped.
 * @param index_1,  The index of the first value being swapped.
 * @param index_2,  The index of the second value being swapped.
 */
void sortSwap(struct List* list, int index_1, int index_2)
{
    int temp;

    temp = list->entities[index_1];
    list->entities[index_1] = list->entities[index_2];
    list->entities[index_2] = temp;
}

/** This function is the recursive part of the quick sort algorithm.
 * @param list, The list being sorted.
 * @param low,  The low limit of the section of list being sorted
 * @param high, The high limit/pivot of the section of list being sorted
 */
void quickSort(struct List* list, int low, int high)
{
    if (high > low)
    {
        int pivotPoint = partition (list, low, high);
        quickSort(list, low, pivotPoint - 1);
        quickSort(list, pivotPoint + 1, high);
    }
}

/** This function splits a function into two parts, greater than the pivot and lower than or equal to the pivot.
 * @param list, The list being sorted.
 * @param low,  The low limit of the section of list being sorted
 * @param high, The high limit/pivot of the section of list being sorted
 */
int partition(struct List* list, int low, int high)
{
    int pivotPoint = (low+ high)/2;
    int pivot = list->entities[pivotPoint];
    sortSwap(list, low, pivotPoint);
    pivotPoint = low;
    for (int i = low + 1; i <= high; ++i)
    {
        if (list->entities[i] <= pivot)
        {
            sortSwap(list, i, ++pivotPoint);
        }
    }
    sortSwap(list, low, pivotPoint);
    return pivotPoint;
}
