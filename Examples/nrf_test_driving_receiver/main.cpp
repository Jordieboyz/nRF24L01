#include "nrf24L01.hpp"
#include "registers.hpp"
#include "pulse.hpp"
#include "hwlib.hpp"

int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto red_led = target::pin_out(target::pins::d3);

    auto echo = target::pin_in(target::pins::d4);
    auto trig = target::pin_out(target::pins::d5);

    auto motor = target::pin_out(target::pins::d6);
    auto grn_led = target::pin_out(target::pins::d7);

    auto ce   = target::pin_out(target::pins::d8);
    auto csn  = target::pin_out(target::pins::d9);    
    auto mosi = target::pin_out(target::pins::d10);
    auto miso = target::pin_in(target::pins::d11);
    auto sclk = target::pin_out(target::pins::d12);
    
    auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk, mosi, miso);
    
    NRF24 RF24 = NRF24(spi_bus, ce, csn);
    pulse ping = pulse(trig, echo, 200);

    std::array<uint8_t, 5> adress = { 0xAA, 0xFF, 0xFF, 0xFF, 0xFF };

    RF24.start_up_chip();
    RF24.write_adress_RX(0, adress);
    RF24.RX_mode();

    std::array<uint8_t, 32> payloadR;

    for(;;)
    {   
        int distance = ping.pulseIn();
       
        RF24.read( payloadR );
        if(payloadR[0] == 1){
            if(distance > 0 && distance < 20){
                motor.write(0);
                grn_led.write(0);
                red_led.write(1);
            }
            else{
                motor.write(1);
                grn_led.write(1);
                red_led.write(0);
            }
        }
        else{
            if(distance > 0 && distance < 20){
                motor.write(0);
                grn_led.write(0);
                red_led.write(1);
            }
            else{
                motor.write(0);
                grn_led.write(1);
                red_led.write(0);
            }
        }
    }
}
