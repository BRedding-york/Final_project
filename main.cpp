#include "mbed.h"
#include <chrono>
#include <cstdio>
//#include <ctime>


class Servos_
{
    private:
        int position_;
        std::chrono::microseconds onTime_;
        std::chrono::microseconds offTime_;
        DigitalOut out_;
        Timeout timer;

    public:
        Servos_(int position, PinName pin) :
            out_(pin)
        {
            *this = position;
        }

        /** Set the servo pin low.
         */
        void off()
        {
            out_ = false;
        }

        /** Set the servo pin high for it's on period.
         */
        void on()
        {
            out_ = true;
            timer.attach( callback( this, &Servos_::off), onTime_);
        }

        /** Getter method for the position_ member.
         */
        int getPosition()
        {
            return position_;
        }

        /** Getter method for the onTime_ member.
         */
        std::chrono::microseconds getOnTime()
        {
            return onTime_;
        }

        /** Getter method for the offTime_ member.
         */
        std::chrono::microseconds getOffTime()
        {
            return offTime_;
        }

        /** Overload of the = operator, updates the position_ and timings of the servo to the new position given
         *  
         *  @param position The new position this servo will be set to. 0 <= position <= 100
         */
        void operator = (int position) 
        {
            if(position <= 100)
            {
                if(position >= 0)
                {
                    position_ = (position);
                }else 
                {
                    position = 0;
                }
            }else
            {
                position_ = 100;
            }
            onTime_  = ((20us*position) + 500us);
            offTime_ = (20ms-onTime_);
        }

        /* This function is just to give the user options on how to update the position 
         * of the servo to match their coding style
         */
        void setPosition(int position)
        {
            *this = position;
        }

        /** Main run loop
         */
        void run()
        {
            timer.attach(callback(this, &Servos_::run), 20ms);
            this->on();
        }
};

// main() runs in its own thread in the OS
int main()
{
    //Thread servoThread;

    Servos_ servo(60, D2);
    servo.run();
    while (true)
    {
        
    }
    
    return 0;
}

