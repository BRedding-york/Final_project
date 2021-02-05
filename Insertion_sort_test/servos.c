#include "servos.h"



//Sorting functions

// n.b. This function may only be used once and if so will be removed from the final edit, since it is only a single for loop.
// This function will take a full servoArrays structure and sort all of the subArrays
void sort_arrays(struct servoArrays arrayOfArrays){
    for(int i = 0; i < arrayOfArrays.noOfArrays; i++){
        sort_insert(arrayOfArrays.servos[i], arrayOfArrays.arraysLen);
    }
}

// This function sorts a subarray of a servoArrays of servos using insertion sort
struct servo* sort_insert (struct servo array[], int arraylen){
    for(int i = 1; i<arraylen; i++){
        int j = i;
        while (j > 0 && array[j-1].timerLen > array[j].timerLen){
            // swap array[j] and array[j-1]
            struct servo temp = array[j-1];
            array[j-1] = array[j];
            array[j] = temp;
            j --;
        }
    }
    return array;
}
