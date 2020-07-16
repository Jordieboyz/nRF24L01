#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

/// @file

void NRF24::flush_RX()
{
    spi.transaction(csn).write( FLUSH_RX );
}

void NRF24::flush_TX()
{
    spi.transaction(csn).write( FLUSH_TX );
}

uint8_t NRF24::read_register(uint8_t reg)
{
    std::array<uint8_t, 1> data_in;

    auto t = spi.transaction(csn); 
    t.write( R_REGISTER | reg);
    t.read(data_in);

    return data_in[0];
}

void NRF24::write_register(uint8_t reg, uint8_t byte)
{
    auto t = spi.transaction(csn);

    t.write(W_REGISTER | reg);
    t.write(byte);
}

void NRF24::set_channel(int channel)
{
    uint8_t Ch = std::min(127, channel);
    write_register( RF_CH, Ch);
}

void NRF24::TX_mode()
{
    uint8_t status = read_register(CONFIG);
    write_register(CONFIG, (status & ~PRIM_RX) | PWR_UP);

    ce.write(1);
    hwlib::wait_ms(10);  
}

void NRF24::RX_mode()
{
    uint8_t status = read_register(CONFIG);
    write_register(CONFIG, status | (PRIM_RX | PWR_UP));

    ce.write(1);   
    hwlib::wait_ms(130);
}

void NRF24::start_up_chip()
{    
    flush_RX();
    flush_TX();

    ce.write(0);
    csn.write(1);

    set_channel(99);
    set_data_rate(dBm_18);
    enable_features();
}  

void NRF24::set_data_rate( dataRate rate )
{
    uint8_t value = read_register( RF_SETUP ) & 0xF9;
    uint8_t data_rate =  rate | value;
    write_register(RF_SETUP, data_rate);
}


void NRF24::enable_features()
{
    write_register( FEATURES, 0x07 );
}
