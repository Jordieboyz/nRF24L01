#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

int main(){
    namespace target = hwlib::target;

    auto echo = target::pin_in(target::pins::d30);
    auto trig    = target::pin_out(target::pins::d32);

    for(;;)
    {
        trig.write(0);
        hwlib:wait_ms(5);

        trig.write(1);
        hwlib:wait_ms(10));
        trig.write(0);

        long duration = echo.read();
        hwlib::cout << duration << hwlib::flush;

    }
}