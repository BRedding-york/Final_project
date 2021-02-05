#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"

int main(){

    int numElements = 10;
    struct List* mylist;

    srand(clock());

    mylist = listConstructor();

    for (int i = 0; i < numElements; ++i)
    {
        listAdd(mylist, rand()%100);
    }
    //listDisplay(mylist);
    printf("start");
    double timestart = (double) clock();
    sortQuick(mylist);
    //sortBubble(mylist);
    double timeend = (double) clock();
    //listDisplay(mylist);

    double timetaken = (timeend - timestart) / CLOCKS_PER_SEC;

    printf("Quick sort took %f seconds", timetaken);
    listDestructor(mylist);
    return 0;
}
