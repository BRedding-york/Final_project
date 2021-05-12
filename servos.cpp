#include "servos.h"
#include <cstdint>

//Methods for the ServoNode class.

ServoList::ServoNode::ServoNode(PinName pinNo, uint16_t index, uint8_t position) :
    out_(pinNo),
    index_(index)
{
    *this = position;
}

void ServoList::ServoNode::setPosition(uint8_t position)
{
    position_ = position;
    std::chrono::microseconds ontime = MINONTIME + ((MAXONTIME - MINONTIME) * position / 256);
    onTime_ = ontime;
    //offTime_ = CYCLETIME - onTime_;
}

void ServoList::ServoNode::operator = (bool switcher)
{
    out_ = switcher;
    if(switcher)
    {
        timer_.attach( callback( this, &ServoNode::off), onTime_);
    }
}

void ServoList::ServoNode::on()
{
    out_ = true;
    timer_.attach( callback( this, &ServoNode::off), onTime_);
}

void ServoList::ServoNode::setTimer(Timeout *timer)
{
    timerptr_ = timer;
}


// Methods for the ServoList class.

ServoList::ServoList(std::chrono::microseconds minOnTime , 
                     std::chrono::microseconds onTimeLen , 
                     std::chrono::microseconds cycleTime ,
                     uint16_t minOnTimeInt ): 
    noOfServos_(0),
    running_(false)
{
    isSorted_ = false;
    counter_ = 0;
    MINONTIME = minOnTime;
    MAXONTIME = onTimeLen;
    GROUPTIME = MINONTIME + MAXONTIME;
    MAXSERVOS = NUMBEROFGROUPS * GROUPSIZE;
    CYCLETIME = cycleTime;
    MINONTIMEINT = minOnTimeInt;
    GROUPSIZE = minOnTimeInt / ITRPTTIME;
    ServoNode::setTimer(&timer_);
}

// Public methods.

int ServoList::add(PinName pinNo, uint8_t position, uint16_t index)
{
    if (noOfServos_ == MAXSERVOS) 
    {
        return 0;
    }
    int quotient = noOfServos_ / GROUPSIZE;
    int remainder = noOfServos_ % GROUPSIZE;
    ServoNode servo(pinNo, index, position);
    list_[quotient][remainder] = servo;
    noOfServos_++;
    isSorted_ = false;
    return 1;
}

int ServoList::remove(int index)
{
    bool removeCheck = false;
    for (int i = 0; i < NUMBEROFGROUPS; i++) 
    {
        for (int j = 0; j < GROUPSIZE; j++) 
        {
            if (list_[i][j].getIndex() == index) 
            {
                __disable_irq();
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
        if(removeCheck)
        {
            __enable_irq();
            sortSorted(i);  // Only one out of place.
        }
    }
    if (removeCheck)    // Something has been removed
    {
        noOfServos_--;
        return 1;
    }
    return 0;           // Nothing has been removed (failed to find servo in list)
}
void ServoList::start()
{
    if(!running_)
    {
        running_ = true;
        counter_ = 0;
        run();
    }
}

void ServoList::end()
{
    running_ = false;
}

void ServoList::updatePosition(uint16_t index, uint8_t position)
{
    bool found = false;
    for (int i = 0; (i < NUMBEROFGROUPS) && (!found); i++) //Search the list until it's found
    {
        for (int j = 0; (j < GROUPSIZE) && (!found); j++)  //Search each group until it's found
        {
            if (list_[i][j].getIndex() == index) 
            {
                list_[i][j] = position;
                found = true;
            }
        }
        if(found)
        {
            sortSorted(i);
        }
    }
}

void ServoList::updateIndex(uint16_t oldIndex, uint16_t newIndex)
{
    bool found = false;
    for (int i = 0; (i < NUMBEROFGROUPS) && (!found); i++) //Search the list until it's found
    {
        for (int j = 0; (j < GROUPSIZE) && (!found); j++)  //Search each group until it's found
        {
            if (list_[i][j].getIndex() == oldIndex) 
            {
                list_[i][j].setIndex(newIndex);
                found = true;
            }
        }
    }
}

uint8_t ServoList::getPosition(uint16_t index)
{
    for (int i = 0; (i < NUMBEROFGROUPS); i++) //Search the list 
    {
        for (int j = 0; (j < GROUPSIZE); j++)  //Search each group 
        {
            if (list_[i][j].getIndex() == index) 
            {
                return list_[i][j].getPosition();   // index found, heres it's position
            }
        }
    }
    return NULL;    // index not found, error.
}

// Private methods.

void ServoList::run()
{
    if(!isSorted_)
    {
        int groups = noOfServos_ / GROUPSIZE;
        for(int i = 0; i < groups - 1; i++)
        {
            sortUnsorted(i);
        }
    }
    __disable_irq();    // This is the most crucial part for the timing, nothing can interrupt this.
    if(running_)
    {
        timer_.attach( callback( this, &ServoList::run), CYCLETIME);     // Start the process again in 20ms
        int groups = noOfServos_ / GROUPSIZE;
        for(int i = 0; i < groups - 1; i++)                             // Turn on each group sequentially 
        {
            timer_.attach( callback( this, &ServoList::nextGroupOn), GROUPTIME*(i+1));       
        }
    }
    __enable_irq();
}

void ServoList::nextGroupOn()
{
    int groups = noOfServos_ / GROUPSIZE;    // How many groups are currently held.
    groupOn(counter_);
    if(counter_ < groups)
    {
        counter_++;
    } else {
        counter_ = 0;
    }
}

void ServoList::groupOn(int groupNo)
{
    int NoServos = GROUPSIZE;
    int groups = noOfServos_ / GROUPSIZE;    // How many groups are currently held.
    if(groupNo == groups - 1)
    {
        NoServos = noOfServos_ % GROUPSIZE;  // If the end group, find the number in the group.
    } 
    if(!isSorted_)
    {
        sortSorted(groupNo);
    }
    for(int j = 0; ; j++)
    {
        if(list_[groupNo][j].getPosition() != NULL) // There should never be a NULL value if everything is done right, does this need to be here?
        {
            list_[groupNo][j].on();
            wait_us(ITRPTTIME);     // Wait for the time taken for an ISR to complete so the off ISRs don't clash on servos with close times.
        }
    }
}

void ServoList::sortSorted(int groupNo)
{
    bool changed;
    int numEntities;
    
    if(groupNo < (noOfServos_ / GROUPSIZE))
    {
        numEntities = GROUPSIZE;                // Full group of servos
    } else
    {
        numEntities = noOfServos_ % GROUPSIZE;   // Find the number of servos in the last group
    }

    __disable_irq();
    do
    {
        changed = false;
        for(int i = 1; i < numEntities; i++)
        {
            if(list_[groupNo][i].getOnTime() > list_[groupNo][i+1].getOnTime()) // Are they in the wrong order?
            {                                                                   // Then swap them.
                ServoNode temp = list_[groupNo][i];
                list_[groupNo][i] = list_[groupNo][i+1];
                list_[groupNo][i+1] = temp;
                changed = true;         // Something has changed don't end sorting
            }
        }
    }while ( !changed ); // Stops when sorting is complete. i.e. when nothing has changed on a full pass.
    isSorted_ = true;
    __enable_irq();
}

void ServoList::sortUnsorted(int groupNo)
{
    int added;
    int numEntities;
    if (groupNo < (noOfServos_ / GROUPSIZE)) 
    {
        numEntities = GROUPSIZE;                // Full group of servos.
    } else 
    {
        numEntities = noOfServos_ % GROUPSIZE;   // Find number of servos in the last group.
    }
    __disable_irq();

    for (int i = 0; i < numEntities; i++) 
    {
        ServoNode temp = list_[groupNo][i]; // Pick up and 'hold' servo
        added = 0;
        for (int j = i - 1; j >= 0; j--) 
        {
            if (added == 0) 
            {
                if (temp.getOnTime() > list_[groupNo][j].getOnTime()) // Is it longer than the held servo?
                {
                    list_[groupNo][j + 1] = list_[groupNo][j];  // Move up next servo in list
                } else 
                {
                    list_[groupNo][j] = temp;                   // Put down held servo
                    added = 1;
                }
            }
        }
    }
    __enable_irq();
    isSorted_ = true;
}


