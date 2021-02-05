#include <stdio.h>
#include <stdlib.h>

#include "servos.h"

int main(){
    struct servoArrays array;
    array.noOfArrays = 9;
    array.arraysLen = 6;
    array.servos = malloc(sizeof(struct servo*) * array.noOfArrays);
    for(int i = 0; i< array.noOfArrays; i++){
        array.servos[i] = malloc(sizeof(struct servo) * array.arraysLen);
        for(int j = 0; j<array.arraysLen; j++){
            array.servos[i][j].pinNo = i*10+j;
            array.servos[i][j].timerLen = ((double)rand())/1000;
        }
    }
    printf("unsorted: \n");
    for(int i = 0; i< array.noOfArrays; i++){
        printf("Array %d: ", i);
        for(int j = 0; j<array.arraysLen; j++){
            printf("pin %d: %lf ", array.servos[i][j].pinNo, array.servos[i][j].timerLen);
        }
        printf("\n");
    }

    printf("\nsorted:\n");
    sort_arrays(array);
    for(int i = 0; i< array.noOfArrays; i++){
        printf("Array %d: ", i);
        for(int j = 0; j<array.arraysLen; j++){
            printf("pin %d: %lf ", array.servos[i][j].pinNo, array.servos[i][j].timerLen);
        }
        printf("\n");
    }

    return 0;
}
