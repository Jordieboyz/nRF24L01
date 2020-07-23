#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto button = target::pin_in(target::pins::d6);
    auto checkON = target::pin_out(target::pins::d7);

    auto ce   = target::pin_out(target::pins::d8);
    auto csn  = target::pin_out(target::pins::d9);    
    auto mosi = target::pin_out(target::pins::d10);
    auto miso = target::pin_in(target::pins::d11);
    auto sclk = target::pin_out(target::pins::d12);
    
    auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk, mosi, miso);
    NRF24 RF24 = NRF24(spi_bus, ce, csn);

    std::array<uint8_t, 5> adress = { 0xAA, 0xFF, 0xFF, 0xFF, 0xFF };

    RF24.start_up_chip();
    RF24.write_adress_TX(adress);
    RF24.TX_mode();

    checkON.write(1);

    for(;;)
    {
        bool button_state = button.read();
        std::array<uint8_t, 32> payload = { !button_state };
        RF24.write_noack( payload );
    }
}
