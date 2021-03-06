#include "nrf24L01.hpp"
#include "registers.hpp"
#include "pulse.hpp"
#include "hwlib.hpp"

int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto led    = target::pin_out(target::pins::d38);
    auto checkON = target::pin_out(target::pins::d2);

    auto trig = target::pin_out(target::pins::d24);
    auto echo = target::pin_in(target::pins::d22);

    auto ce_1   = target::pin_out(target::pins::d8);
    auto csn_1  = target::pin_out(target::pins::d9);    
    auto mosi_1 = target::pin_out(target::pins::d10);
    auto miso_1 = target::pin_in(target::pins::d11);
    auto sclk_1 = target::pin_out(target::pins::d12);
    
    auto spi_bus_1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk_1, mosi_1, miso_1);

    auto ce_2   = target::pin_out(target::pins::d3);
    auto csn_2  = target::pin_out(target::pins::d4);    
    auto mosi_2 = target::pin_out(target::pins::d5);
    auto miso_2 = target::pin_in(target::pins::d6);
    auto sclk_2 = target::pin_out(target::pins::d7);
    auto spi_bus_2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk_2, mosi_2, miso_2);
    
    NRF24 RF24_1 = NRF24(spi_bus_1, ce_1, csn_1);
    NRF24 RF24_2 = NRF24(spi_bus_2, ce_2, csn_2);
    pulse ping = pulse(trig, echo, 200);

    std::array<uint8_t, 5> adress = { 0x34, 0xFF, 0xFF, 0xFF, 0xFF };

    RF24_1.start_up_chip();
    RF24_2.start_up_chip();

    RF24_1.write_adress_TX(adress);
    RF24_1.TX_mode();

    RF24_2.write_adress_RX(0, adress);
    RF24_2.RX_mode();   

    std::array<uint8_t, 32> payloadR;

    for(;;)
    {
        uint8_t distance = ping.pulseIn();
        std::array<uint8_t, 32> payload = { distance };

        RF24_1.write_noack( payload );


        RF24_2.read( payloadR );

        if(payloadR[0] < 30 && payloadR[0] > 0){
            led.write(1);
            checkON.write(0);
        }
        else{
            checkON.write(1);
            led.write(0);
        }

    }
}
