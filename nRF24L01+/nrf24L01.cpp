#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

/// @file

//==================================================================
                        //PROTECTED FUNCTIONS                     
//==================================================================

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

void NRF24::read_register(uint8_t reg, std::array<uint8_t, 5> & bytes)
{
    auto trans = spi.transaction(csn); 

    trans.write( R_REGISTER | reg);
    trans.read(bytes);
}

void NRF24::write_register(uint8_t reg, std::array<uint8_t, 5> & bytes)
{
    auto trans = spi.transaction(csn);

    trans.write(W_REGISTER | (0x20 | reg));
    for(unsigned int i = 0; i < bytes.size(); i++)
    {
        trans.write(bytes[i]);
    }
}

void NRF24::read_payload(std::array<uint8_t, 32> & payload)
{
    auto trans = spi.transaction(csn);

    trans.write( R_RX_PAYLOAD );
    trans.read( payload );
}

void NRF24::write_payload(std::array<uint8_t, 32> & payload)
{
    auto trans = spi.transaction(csn);

    trans.write( W_TX_NO_ACK );
    for(unsigned int i = 0; i < payload.size(); i++ )
    {
        trans.write(payload[i]);
    }
}

void NRF24::flush_RX()
{
    spi.transaction(csn).write( FLUSH_RX );
}

void NRF24::flush_TX()
{
    spi.transaction(csn).write( FLUSH_TX );
}

void NRF24::enable_features()
{
    write_register( FEATURES, 0x07 );
}

void NRF24::reset_enabled_pipes()
{
    write_register( EN_RXADDR, 0x03 );
}

///=================================================================
                        //PUBLIC FUNCTIONS                        
//==================================================================

void NRF24::start_up_chip()
{    
    flush_RX();
    flush_TX();

    ce.write(0);
    csn.write(1);

    reset_enabled_pipes();
    set_channel(99);
    set_data_rate(dBm_18);
    enable_features();
}

void NRF24::set_channel(int channel)
{
    uint8_t Ch = std::min(127, channel);
    write_register( RF_CH, Ch);
}

void NRF24::set_data_rate( dataRate rate )
{
    uint8_t value = read_register( RF_SETUP ) & 0xF9;
    uint8_t data_rate =  rate | value;
    write_register(RF_SETUP, data_rate);
}

void NRF24::RX_mode()
{
    uint8_t status = read_register(CONFIG);
    write_register(CONFIG, status | (PRIM_RX | PWR_UP));

    ce.write(1);   
    hwlib::wait_ms(130);
}

void NRF24::TX_mode()
{
    uint8_t status = read_register(CONFIG);
    write_register(CONFIG, (status & ~PRIM_RX) | PWR_UP);

    ce.write(1);
    hwlib::wait_ms(10);  
}

void NRF24::write_adress_TX(std::array<uint8_t, 5> & adress)
{
    write_register(TX_ADDR,     adress);
    write_register(RX_ADDR_P0,  adress);
    write_register(RX_PW_P0,    data_size);
} 

void NRF24::write_adress_RX(uint8_t pipe, std::array<uint8_t, 5> & adress)
{
    uint8_t LSBAdress = adress[0];

    if(pipe == 0)
    {
        write_register(pipeAdress[pipe], adress);
        write_register(pipeWidth[pipe], data_size);
    } else if(pipe == 1)
    {
        write_register(pipeAdress[pipe], adress);
        write_register(pipeWidth[pipe], data_size);
    } else
    {
        write_register(pipeAdress[pipe], LSBAdress);
        write_register(pipeWidth[pipe], data_size);
        write_register(EN_RXADDR, (read_register(EN_RXADDR) & 0xFF) | (0x01 << pipeEnable[pipe]));
    }
}

void NRF24::read(std::array<uint8_t, 32> & payload )
{   
    read_payload( payload ); 
}

void NRF24::write_noack(std::array<uint8_t, 32> & payload)
{
    write_payload( payload );
}

