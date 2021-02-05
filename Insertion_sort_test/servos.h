/* Contains all the information needed for each servo
 * pinNo    The number pin that this servo is connected to.
 * timerLen The amount of time that the pin needs to be high for this servo
 */
struct servo{
    int pinNo;
    double timerLen;
};

struct servoArrays{
    int arraysLen;
    int noOfArrays;
    struct servo** servos;
};

// Function declarations

/**This function will take a full servoArrays structure and sort all of the subArrays
n.b. This function may only be used once and if so will be removed from the final edit, since it is only a single for loop.
 * @param arrayOfArrays[]   The servoArrays to be sorted
 *
 * @return                  The sorted servoArrays struct
 */
void sort_arrays(struct servoArrays arrayOfArrays);

/**This function sorts a subarray of a servoArrays of servos using insertion sort
 * @param array[]   The array of servos
 * @param arrayLen  The length of the array of servos
 *
 * @return          The sorted array of servos
 */
struct servo* sort_insert (struct servo array[], int arraylen);
