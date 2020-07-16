#include "nrf24L01.hpp"
#include "registers.hpp"
#include "hwlib.hpp"

void NRF24::print_registers()
{
    hwlib::cout << "\nSOME REGISTERS!\n\n" << hwlib::hex << hwlib::setfill( '0' )
                << "Config      = 0x" << hwlib::setw( 2 ) << read_register( CONFIG )        << "\n"
                << "AutoAck     = 0x" << hwlib::setw( 2 ) << read_register( EN_AA )         << "\n"
                << "EnabledREG  = 0x" << hwlib::setw( 2 ) << read_register( EN_RXADDR )     << "\n"
                << "setup_aw    = 0x" << hwlib::setw( 2 ) << read_register( SETUP_AW )      << "\n"
                << "setup RETR  = 0x" << hwlib::setw( 2 ) << read_register( SETUP_RETR )    << "\n"
                << "channel     = 0x" << hwlib::setw( 2 ) << read_register( RF_CH )         << "\n"
                << "SetupRF     = 0x" << hwlib::setw( 2 ) << read_register( RF_SETUP )      << "\n"
                << "StatusREG   = 0x" << hwlib::setw( 2 ) << read_register( STATUS )        << "\n"
                << "OBSESRVE    = 0x" << hwlib::setw( 2 ) << read_register( OBSERVE_TX )    << "\n"
                << "FIFOstatus  = 0x" << hwlib::setw( 2 ) << read_register( FIFO_STATUS )   << "\n"
                << hwlib::flush;
}

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

void NRF24::setChannel(int channel)
{
    uint8_t Ch = std::min(127, channel);
    write_register( RF_CH, Ch);
}

void NRF24::powerDown_mode()
{    uint8_t status = read_register(CONFIG);
    write_register(CONFIG, status & ~PWR_UP);
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

void NRF24::startUpChip()
{    
    flush_RX();
    flush_TX();

    ce.write(0);
    csn.write(1);

    setChannel(99);
    setDataRate();
    enable_features();
}  

void NRF24::setDataRate()
{
    uint8_t value = read_register( RF_SETUP ); 
    write_register(RF_SETUP, (value & 0xF9));
}

void NRF24::enable_features()
{
    write_register( FEATURES, 0x07 );
}