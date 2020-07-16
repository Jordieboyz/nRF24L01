#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

void print(NRF24 RF24, uint8_t reg){
    hwlib::cout << hwlib::hex << hwlib::setfill( '0' ) << "0x" << hwlib::setw( 2 ) << RF24.read_register( reg )   << "\n" << hwlib::flush;
}

void print_addr(NRF24 RF24, uint8_t reg ){
    std::array<uint8_t, 5> addr;
    RF24.read_register(reg, addr);

    for(uint8_t data : addr){
        hwlib::cout << hwlib::hex << hwlib::setfill( '0' ) << "0x" << hwlib::setw( 2 ) << data << " " << hwlib::flush;
    }
    hwlib::cout << "\n" << hwlib::flush;
}

int main(){
    hwlib::wait_ms(1'000);
    namespace target = hwlib::target;

    auto button = target::pin_in(target::pins::d7);
    auto led    = target::pin_out(target::pins::d28);

    auto ce_1   = target::pin_out(target::pins::d8);
    auto csn_1  = target::pin_out(target::pins::d9);    
    auto mosi_1 = target::pin_out(target::pins::d10);
    auto miso_1 = target::pin_in(target::pins::d11);
    auto sclk_1 = target::pin_out(target::pins::d12);
    
    auto spi_bus_1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk_1, mosi_1, miso_1);

    auto ce_2   = target::pin_out(target::pins::d2);
    auto csn_2  = target::pin_out(target::pins::d3);    
    auto mosi_2 = target::pin_out(target::pins::d4);
    auto miso_2 = target::pin_in(target::pins::d5);
    auto sclk_2 = target::pin_out(target::pins::d6);
    auto spi_bus_2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(sclk_2, mosi_2, miso_2);
    
    NRF24 RF24_1 = NRF24(spi_bus_1, ce_1, csn_1);
    NRF24 RF24_2 = NRF24(spi_bus_2, ce_2, csn_2);

    std::array<uint8_t, 5> adress = { 0x34, 0xFF, 0xFF, 0xFF, 0xFF };
    std::array<uint8_t, 32> payloadR;

    RF24_1.startUpChip();
    RF24_2.startUpChip();

    RF24_1.openWritingPipe(adress);
    RF24_1.TX_mode();

    RF24_2.openReadingPipe(0, adress);
    RF24_2.RX_mode();   

    std::array<hwlib::string<3>, 2> mode = { "AAN", "UIT" };
    std::array<uint8_t, 32> payload;

    for(;;)
    {
        RF24_1.flush_TX();
        RF24_2.flush_RX();

        bool button_state = button.read();

        if(!button_state){
            RF24_1.write( payload, mode[0] );
        }
        else{
            RF24_1.write( payload, mode[1] );
        }
        
        
        if(RF24_2.read_payload( payloadR ) == "AAN"){
            led.write(1);
        }
        else{
            led.write(0);
        }
    }
}