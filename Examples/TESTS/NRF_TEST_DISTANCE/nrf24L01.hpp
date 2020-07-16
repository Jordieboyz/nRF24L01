#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"
#include "registers.hpp"


class NRF24 {
private:
    hwlib::spi_bus & spi;
    hwlib::pin_out & ce;
    hwlib::pin_out & csn;
    uint8_t data_size;    

public:
    NRF24(hwlib::spi_bus & spi, hwlib::pin_out & ce, hwlib::pin_out & csn) :
        spi( spi ), ce( ce ), csn( csn ), data_size( 32 )
    {}

    enum pipeInfo : uint8_t {
        RX_ADDR_P0 = 0x0A, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5,
        RX_PW_P0 = 0x11, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5 
    };

    void enable_features();
    // void setRetries();
    void setDataRate();
    void flush_RX();
    void flush_TX();
    void powerDown_mode();
    void RX_mode();
    void TX_mode();
    void print_registers();
    void startUpChip();
    void setChannel(int channel);
    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t byte);

    template< size_t n >
    void read_register(uint8_t reg, std::array<uint8_t, n> & bytes)
    {
        auto trans = spi.transaction(csn); 

        trans.write( R_REGISTER | reg);
        trans.read(bytes);
    }

    template< size_t n >
    void write_register(uint8_t reg, std::array<uint8_t, n> & bytes){
        auto trans = spi.transaction(csn);

        trans.write(W_REGISTER | (0x20 | reg));
        for(unsigned int i = 0; i < bytes.size(); i++){
            trans.write(bytes[i]);
        }
    }

    template< size_t n >                            
    void openWritingPipe(std::array<uint8_t, n> & adress)
    {
        write_register(TX_ADDR,     adress);
        write_register(RX_ADDR_P0,  adress);
        write_register(RX_PW_P0,    data_size);
    } 


    const uint8_t pipeEnable[6] = { ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5 };
    const uint8_t pipeAdress[6] = { RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5 };
    const uint8_t pipeWidth[6]  = { RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5 };

    template< size_t n >
    void openReadingPipe(uint8_t pipe, std::array<uint8_t, n> & adress)
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
            write_register(pipeEnable[pipe], (read_register(EN_RXADDR) | pipeEnable[pipe]));
        }
    }

    template< size_t n >
    void write_payload(std::array<uint8_t, n> & payload){
        auto trans = spi.transaction(csn);

        trans.write( W_TX_NO_ACK );
        for(unsigned int i = 0; i < payload.size(); i++ ){
            trans.write(payload[i]);
        }
    }

    template< size_t n>
    hwlib::string<n> read_payload(std::array<uint8_t, n> & payload){
        auto trans = spi.transaction(csn);

        trans.write( R_RX_PAYLOAD );
        trans.read( payload );

        hwlib::string<3> a = "";
        for(auto data : payload){
            a += (char)data;
        }
        return a;
    }

    template< size_t n>
    void write(std::array<uint8_t, n> & payload, hwlib::string<3> mode)
    {
        for(unsigned int i = 0; i < payload.size(); i++){
            payload[i] = mode[i];
        }

        write_payload( payload );
        hwlib::wait_ms(20);
    }
};



#endif // NRF24_HPP