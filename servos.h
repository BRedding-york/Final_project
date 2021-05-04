#include "mbed.h"
#include <stdlib.h>
#include <cstdint>
#include <cstdio>
#include <chrono>


/** Servo list class 
 *  2D array list type
 *  This data type removes any dynamic memory allocation.
 */
class ServoList 
{
private:
    /** Single servo data structure
    *  
    */
    class ServoNode
    {
    private:
        uint8_t position_;                      // position between 0 and 256.
        uint16_t index_;                         // Index of the servo according to the user.
        std::chrono::microseconds onTime_;      // Length of time the servo is on for.
        std::chrono::microseconds offTime_;     // Length of time the servo is off for. Do I need this?
        DigitalOut out_;                        // The pin that the servo is attached to.
        Timeout* timerptr_;                     // The pointer to the Timeout for callbacks

    public:
        /** Constructor for servoNode class
        *  Not intended for use seperate from ServoList class
        * @param pinNo, The pin name for the digital out attached to the servo.
        * @param index, The index specified by the user.
        * @param timerptr, The pointer to the Timeout for callbacks.
        * @param position, position to initialised the servo to. Default to 128 for no input.
        */
        ServoNode(PinName pinNo, uint16_t index, uint8_t position = 128) :
            out_(pinNo),
            index_(index)
        {
            *this = position;
        }

        /** Returns the value of the index_ variable.
        */
        uint8_t getIndex()
        {
            return index_;
        }

        /** Updates the index_ variable.
        */
        void setIndex(uint8_t index)
        {
            index_ = index;
        }

        /** Returns the value of the onTime_ variable.
        */
        std::chrono::microseconds getOnTime()
        {
            return onTime_;
        }

        /**
        */
        int getPosition()
        {
            return position_;
        }
        
        /** Updates the position_, onTime_ and offTime_ variables.
        */
        void setPosition(uint8_t position)
        {
            position_ = position;
            std::chrono::microseconds ontime = MINONTIME + ((ONTIMELEN - MINONTIME) * position / 256);
            onTime_ = ontime;
            offTime_ = CYCLETIME - onTime_;
        }

        /** operator overload that allows a quick way to use setPosition method.
        */
        void operator = (uint8_t position)
        {
            setPosition(position);
        }

        /** Turn on the DigitalOut out_ variable.
        */
        void on()
        {
            out_ = true;
            timer.attach( callback( this, &ServoNode::off), onTime_);
        }

        /** Turn off the DigitalOut out_variable.
        */
        void off()
        {
            out_ = false;
        }

        /** Either turns out_ on or off.
        */
        void operator = (bool switcher)
        {
            out_ = switcher;
            if(switcher)
            {
                timer.attach( callback( this, &ServoNode::off), onTime_);
            }
        }
    };

// ServoList class starts here

    /* Static member variable*/
    static std::chrono::microseconds CYCLETIME;                     // The length of time before the next cycle will start.
    static std::chrono::microseconds MINONTIME;                     // The minimum on time for these servos.
    static std::chrono::microseconds GROUPTIME;                     // Length of time taken to guarentee no clashes.
    static std::chrono::microseconds ONTIMELEN;                     // The maximum on time for these servos.
    static uint16_t MINONTIMEINT;
    static uint8_t GROUPSIZE;                                       // Number of servos in each group. MINONTIME must be put manually in here
    static uint16_t MAXSERVOS;                                      // The total number of servos that can be stored.
    static const uint8_t ITRPTTIME = 100;                           // Length of time taken to service interrupt in us.
    static const uint8_t NUMBEROFGROUPS = 6;                        // Number of groups possible.
    static Timeout timer;                                           // Timeout that all callbacks use

    /* Non-static member variables*/
    int noOfServo_;                     // no of servos currently held in the list.
    ServoNode *list_[NUMBEROFGROUPS];   // 2D array-type list containing the servo data.
    bool running_;                      // Switch for running the main loop.

    
    /** The main function of this program. Turns on all servo pins.
     */
    void run()
    {
        __disable_irq();
        if(running_)
        {
            timer.attach( callback( this, &ServoList::run), CYCLETIME);      // Start the process again in 20ms
            int groups = noOfServo_ / GROUPSIZE;
            for(int i = 0; i < groups - 1; i++)
            {
                timer.attach( callback( this, &ServoList::onGroup(i)), GROUPTIME*(i+1));       
            }
        }
        __enable_irq();
    }

    void onGroup(int groupNo)
    {
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
                wait_us(ITRPTTIME);
            }
        }
    }

    /** Sorts a group of servo motors that is mostly sorted.
     * Uses Bubble sort.
     */
    void sortSorted(int groupNo) 
    {
        bool changed;
        int numEntities;
        if(groupNo < (noOfServo_ / GROUPSIZE))
        {
            numEntities = GROUPSIZE;
        } else
        {
            numEntities = noOfServo_ % GROUPSIZE;
        }

        do
        {
            changed = false;
            for(int i = 1; i < numEntities; i++)
            {
                __disable_irq();
                if(list_[groupNo][i].getOnTime() > list_[groupNo][i+1].getOnTime())
                {
                    ServoNode temp = list_[groupNo][i];
                    list_[groupNo][i] = list_[groupNo][i+1];
                    list_[groupNo][i+1] = temp;
                    changed = true;
                }
                __enable_irq();
            }
        }while ( !changed ); // Stops when sorting is complete. i.e. when nothing has changed on a full pass.
    }

    /** Sorts a completely unsorted list
     *  Uses Insertion sort
     */
    void sortUnsorted(int groupNo)
    {
        int added;
        int numEntities;

        if (groupNo < (noOfServo_ / GROUPSIZE)) 
        {
            numEntities = GROUPSIZE;
        } else 
        {
            numEntities = noOfServo_ % GROUPSIZE; 
        }

        for (int i = 0; i < numEntities; i++) 
        {
            ServoNode temp = list_[groupNo][i];
            added = 0;
            for (int j = i - 1; j >= 0; j--) 
            {
                if (added == 0) 
                {
                    if (temp.getOnTime() > list_[groupNo][j].getOnTime()) 
                    {
                        list_[groupNo][j + 1] = list_[groupNo][j];
                    } else 
                    {
                        list_[groupNo][j] = temp;
                        added = 1;
                    }
                }
            }
        }
    }

public:
    /** Constructor method for ServoList class
     *  @param minOnTime, The minimum on time for your servos. Default 500us
     *  @param onTimeLen, The maximum on time for your servos. Default 2.5ms
     *  @param cycleTime, The full on time + off time for your servos. Default 20ms
     */
    ServoList(std::chrono::microseconds minOnTime = 500us , 
              std::chrono::microseconds onTimeLen = 2500ms, 
              std::chrono::microseconds cycleTime = 20ms,
              uint16_t minOnTimeInt = 500) : 
        noOfServo_(0),
        running_(false)
    {
        MINONTIME = minOnTime;
        ONTIMELEN = onTimeLen;
        GROUPTIME = MINONTIME + ONTIMELEN;
        CYCLETIME = cycleTime;
        MINONTIMEINT = minOnTimeInt;
        GROUPSIZE = minOnTimeInt / ITRPTTIME;
        MAXSERVOS = NUMBEROFGROUPS * GROUPSIZE;
    }

    /** Appends a new servo to the list of servos, iff maximum number of servos not
     * reached.
     */
    int add(PinName pinNo, uint8_t position, uint16_t index)
    {
        if (noOfServo_ == MAXSERVOS) 
        {
            return 0;
        }
        int quotient = noOfServo_ / GROUPSIZE;
        int remainder = noOfServo_ % GROUPSIZE;
        ServoNode servo(pinNo, index, position);
        list_[quotient][remainder] = servo;
        noOfServo_++;
        return 1;
    }

    /** Removes the servo with the correct pinNo, fills the gap by pulling the
     * rest of the data forwards one.
     */
    int remove(int index) 
    {
        bool removeCheck = false;
        for (int i = 0; i < NUMBEROFGROUPS; i++) 
        {
            for (int j = 0; j < GROUPSIZE; j++) 
            {
                if (list_[i][j].getIndex() == index) 
                {
                    removeCheck = true;
                    if (j < GROUPSIZE - 1) 
                    {
                        list_[i][j] = list_[i][j + 1];      // Move next servo down.
                        list_[i][j + 1].setIndex(index);    // Allow to repeat on next servo.
                        list_[i][j + 1].setPosition(NULL);  // Make the last servo not turn on in the run function.
                    } else 
                    {
                        list_[i][j] = list_[i + 1][0];      // Move next servo down (from next group).
                        list_[i + 1][0].setIndex(index);    // Allow to repeat on next servo (in next group).
                        list_[i + 1][0].setPosition(NULL);  // Make the last servo not turn on in the run function.
                    }
                }
            }
            sortSorted(i);  // Only one out of place.
        }
        if (removeCheck) 
        {
            noOfServo_--;
            return 1;
        }
        return 0;
    }

    /** Entry point to start the main loop.
     */
    void start()
    {
        running_ = true;
        run();
    }

    /** Stop running main loop. The next callback will still be called but will end without doing anything.
     */
    void end()
    {
        running_ = false;
    }

    void setCycleTime(std::chrono::microseconds cycleTime)
    {
        CYCLETIME = cycleTime;
    }

    void setMinOnTime(std::chrono::microseconds minOnTime)
    {
        MINONTIME = minOnTime;
        GROUPTIME = ONTIMELEN + MINONTIME;
    }

    void setOnTimeLen(std::chrono::microseconds onTimeLen)
    {
        ONTIMELEN = onTimeLen;
        GROUPTIME = ONTIMELEN + MINONTIME;
    }

    /** Test function for ISR timings once run this will alter the ITRPTTIME variable.
     * turns on each pin as fast as possible, the difference between how fast the pins turn on and turn off will be ITRPTTIME.
     */
    void testISRTimings()
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
    }
};