#include "mbed.h"
#include <stdlib.h>
#include <cstdint>
#include <cstdio>
#include <chrono>


/** Servo list class 
 *  2D array list type
 */
class ServoList 
{
private:
    /** Single servo data structure
    */
    class ServoNode
    {
    private:
        uint8_t position_;                      // position between 0 and 256.
        uint16_t index_;                        // Index of the servo according to the user.
        std::chrono::microseconds onTime_;      // Length of time the servo is on for.
        std::chrono::microseconds offTime_;     // Length of time the servo is off for. Do I need this?
        DigitalOut out_;                        // The pin that the servo is attached to.
        Timeout* timerptr_;                     // The pointer to the Timeout for callbacks

    public:
        /** Constructor for servoNode class
         * Not intended for use seperate from ServoList class
         *
         * @param pinNo, The pin name for the digital out attached to the servo.
         * @param index, The index specified by the user.
         * @param timerptr, The pointer to the Timeout for callbacks.
         * @param position, position to initialised the servo to. Default to 128 for no input.
         */
        ServoNode(PinName pinNo, uint16_t index, uint8_t position = 128);

        /** Returns the value of the index_ variable. */ uint8_t getIndex(){ return index_; }

        /** Returns the value of the onTime_ variable. */ std::chrono::microseconds getOnTime(){ return onTime_; }

        /** Returns the value of the position_ variable.*/ int getPosition(){ return position_; }

        /** Updates the index_ variable.*/ void setIndex(uint8_t index){ index_ = index; }
        
        /** operator overload that allows a quick way to use setPosition method.*/ void operator = (uint8_t position){ setPosition(position); }

        /** Turn off the DigitalOut out_variable. */ void off(){ out_ = false; }

        /** Turn on the DigitalOut out_ variable. */ void on();

        /** Either turns out_ on or off. */ void operator = (bool switcher);
        
        /** Updates the position_, onTime_ and offTime_ variables. */ void setPosition(uint8_t position);
    };

// ServoList class starts here

    /* Static member variable*/
    static std::chrono::microseconds CYCLETIME;                     // The length of time before the next cycle will start.
    static std::chrono::microseconds MINONTIME;                     // The minimum on time for these servos.
    static std::chrono::microseconds GROUPTIME;                     // Length of time taken to guarentee no clashes.
    static std::chrono::microseconds MAXONTIME;                     // The maximum on time for these servos.
    static uint16_t MINONTIMEINT;
    static uint8_t GROUPSIZE;                                       // Number of servos in each group. MINONTIME must be put manually in here
    static uint16_t MAXSERVOS;                                      // The total number of servos that can be stored.
    static const uint8_t ITRPTTIME = 100;                           // Length of time taken to service interrupt in us.
    static const uint8_t NUMBEROFGROUPS = 6;                        // Number of groups possible.
    static Timeout timer;                                           // Timeout that all callbacks use
    static int counter;

    /* Non-static member variables*/
    int noOfServo_;                     // no of servos currently held in the list.
    ServoNode *list_[NUMBEROFGROUPS];   // 2D array-type list containing the servo data.
    bool running_;                      // Switch for running the main loop.

    
    /** The main function of this program. Turns on all servo pins for the correct length of time.
     */
    void run();

    /** Callback wrapper for groupOn. Calls the next groupOn in the rotation and handles the inputs.
     */
    void nextGroupOn();

    /** Smaller sub-method for the run method, turns on the servos in one individual group.
     * @param groupNo, The group of servos to be turned on.
     */
    void groupOn(int groupNo);

    /** Sorts a group of servo motors that is mostly sorted. Uses Bubble sort.
     * @param groupNo, The group of servos to be sorted.
     */
    void sortSorted(int groupNo);

    /** Sorts a completely unsorted list. Uses Insertion sort
     * @param groupNo, The group of servos to be sorted.
     */
    void sortUnsorted(int groupNo);

public:
    /** Constructor method for ServoList class 
     * @param minOnTime, The minimum on time for your servos. Default 500us
     * @param onTimeLen, The maximum on time for your servos. Default 2.5ms
     * @param cycleTime, The full on time + off time for your servos. Default 20ms
     * @param minOnTimeInt, The minimum on time for your servos, in microseconds. !!Must be the same as minOnTime!!. 
     */
    ServoList(std::chrono::microseconds minOnTime = 500us , 
              std::chrono::microseconds onTimeLen = 2500ms, 
              std::chrono::microseconds cycleTime = 20ms,
              uint16_t minOnTimeInt = 500);

    /** Appends a new servo to the list of servos, iff maximum number of servos not reached.
     * @param pinNo, The PinName of the DigitalOut pin attached to the servo
     * @param position, The starting position of the servo
     * @param index, Index of the servo for later reference */
    int add(PinName pinNo, uint8_t position, uint16_t index);

    /** Removes the servo with the correct pinNo, 
     * fills the gap by pulling the rest of the data forwards one. 
     * @param index, The index of the servo to be removed from the list*/ int remove(int index);

    /** Entry point to start the main loop. */ void start();

    /** Stop running main loop. The next callback will start but won't do anything. */ void end();

    /** Update the position_ of servo [index] */ void updatePosition(uint16_t index, uint8_t position);

    /** Update the index_ of servo [oldIndex] */ void updateIndex(uint16_t oldIndex, uint16_t newIndex);

    /** Find the position of servo [index] */ uint8_t getPosition(uint16_t index);
//Getters and setters

    /** Updates the static variable that sets the total time for a full on and off cycle*/
    void setCycleTime(std::chrono::microseconds cycleTime){ CYCLETIME = cycleTime; }

    /** Updates the static variable that sets the minimum time a servo can be on*/
    void setMinOnTime(std::chrono::microseconds minOnTime){ MINONTIME = minOnTime; GROUPTIME = MAXONTIME + MINONTIME; }

    /** Updates the static variable that sets the maximum time a servo can be on*/
    void setOnTimeLen(std::chrono::microseconds maxOnTime){ MAXONTIME = maxOnTime; GROUPTIME = MAXONTIME + MINONTIME; }


// For further work only

    /** Test function for ISR timings once run this will alter the ITRPTTIME variable.
     * turns on each pin as fast as possible, the difference between how fast the pins turn on and turn off will be ITRPTTIME.
     */
    /*void testISRTimings()
    {
        int groupNo = 1;
        int NoServos = GROUPSIZE;
        int groups = noOfServo_ / GROUPSIZE;
        if(groupNo == groups - 1)
        {
            NoServos = noOfServo_ % GROUPSIZE;
        } 
        for(int j = 0; ; j++)
        {
            if(list_[groupNo][j].getPosition() != NULL) // How do I do this?
            {
                list_[groupNo][j].on();
                //wait_us(ITRPTTIME);
            }
        }
    }*/

    
};