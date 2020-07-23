#include "hwlib.hpp"
#include "pulse.hpp"


int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto led = target::pin_in(target::pins::d7);

    auto echopin = target::pin_in(target::pins::d2);
    auto trigpin = target::pin_out(target::pins::d3);

    pulse ping = pulse(trigpin, echopin, 400);

    for(;;){
        int distance = ping.pulseIn();
        hwlib::cout << "distance: " << distance << " cm\n" << hwlib::flush;
        hwlib::wait_ms(50);
    }
}


