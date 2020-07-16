#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto led = target::pin_out(target::pins::d7);
    auto checkON = target::pin_out(target::pins::d3);

    auto ce   = target::pin_out(target::pins::d8);
    auto csn  = target::pin_out(target::pins::d9);    
    auto mosi = target::pin_out(target::pins::d10);
    auto miso = target::pin_in(target::pins::d11);
    auto sclk = target::pin_out(target::pins::d12);
    
    auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk, mosi, miso);
    NRF24 RF24 = NRF24(spi_bus, ce, csn);

    std::array<uint8_t, 5> adress = { 0xAA, 0xFF, 0xFF, 0xFF, 0xFF };

    RF24.start_up_chip();
    RF24.write_adress_RX(0, adress);
    RF24.RX_mode();

    std::array<uint8_t, 32> payloadR;

    checkON.write(1);

    for(;;)
    {
        if(RF24.read( payloadR ) == 1 )
        {
            led.write(1);
        }
        else
        {
            led.write(0);
        }
    }
}
