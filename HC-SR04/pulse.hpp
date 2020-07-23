#ifndef PULSE_HPP
#define PULSE_HPP

#include "hwlib.hpp"


class pulse {
private:
    hwlib::pin_out & trigpin;
    hwlib::pin_in & echopin;
    int max_dist;

public:
    pulse( hwlib::pin_out & trigpin,  hwlib::pin_in & echopin, int max_dist): 
        trigpin( trigpin ), echopin( echopin ), max_dist( max_dist )
    {}

    void trigger_pulse()
    {
        trigpin.write(0);
        hwlib::wait_us(4);
        trigpin.write(1);
        hwlib::wait_us(10);
        trigpin.write(0);
    }

    int pulseIn()
    {
        trigger_pulse();
        while(!echopin.read()){}
            int counter = 0;
        while(echopin.read()){
            counter += 1;
            hwlib::wait_us(1);
        }
        int distance = counter/8;
        if(distance > max_dist){
            distance  = 0;
        }
        return distance;
    }
};

#endif // NewPing