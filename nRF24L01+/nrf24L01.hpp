#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"
#include "registers.hpp"

enum pipeInfo : uint8_t {
    ERX_P0 = 0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5,
    RX_ADDR_P0 = 0x0A, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5,
    RX_PW_P0 = 0x11, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5 
};

enum dataRate : uint8_t { 
    dBm_18 = 0, 
    dBm_12, 
    dBm_6, 
    dBm_0 
}; 

/// @file

/// \brief
/// nRF24L01+ driver
/// \details
/// This driver is a driver for the nRF24L01+
/// It's a fairly easy library using the most significant functions
/// of the nRF24L01+
class NRF24 {
private:
    hwlib::spi_bus & spi;
    hwlib::pin_out & ce;
    hwlib::pin_out & csn;
    uint8_t data_size;

    const uint8_t pipeEnable[6] = { ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5 };
    const uint8_t pipeAdress[6] = { RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5 };
    const uint8_t pipeWidth[6]  = { RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5 };

    /// \brief
    /// Read from a regsiter
    /// \details
    /// This funtion reads one byte from the given register
    uint8_t read_register(uint8_t reg);

    /// \brief
    /// Write to a regsiter
    /// \details
    /// This funtion writes one byte to the given register
    void write_register(uint8_t reg, uint8_t byte);

public:
    /// \brief
    /// constructor nRF24L01+
    /// \details
    /// The constructor of the nRF24L01+ needs a ce and csn pin to work
    NRF24(hwlib::spi_bus & spi, hwlib::pin_out & ce, hwlib::pin_out & csn) :
        spi( spi ), ce( ce ), csn( csn ), data_size( 32 )
    {}

    /// \brief
    /// The startUP of the nRF24L01+
    /// \details
    /// The funtion gets the chip ready to write and read from the nRF24L01+
    /// This funtion also ensures there are correct values in a couple registers
    /// make sure this is the first function you call on the nRF24L01+ 
    void start_up_chip();
    
    /// \brief
    /// Set a channel
    /// \details
    /// This funtion sets the channel whereover the nRF24L01+ will communicate
    void set_channel(int channel);
    
    /// \brief
    /// Set the data rate
    /// \details
    /// This funtion sets the speed of the output data going through the air
    void set_data_rate( dataRate rate );

    /// \brief
    /// Enable the features
    /// \details
    /// This function enables the right featuers on the nRF24L01+ for example
    /// the no_ack functionality
    void enable_features();

    /// \brief
    /// trigger RX_MODE
    /// \details
    /// This function forces the nRF24L01+ to operate in the RX mode
    void RX_mode();
    
    /// \brief
    /// trigger tX_MODE
    /// \details
    /// This function forces the nRF24L01+ to operate in the TX mode
    void TX_mode();
    
    /// \brief
    /// flush RX
    /// \details
    /// This function flushes the RX queue
    void flush_RX();
    
    /// \brief
    /// flush TX
    /// \details
    /// This function flushes the TX queue
    void flush_TX();
    
    /// \brief
    /// read multiple bytes from a single register
    /// \details
    /// This function reads the an x amount of incoming bytes (from the MISO pin)
    template< size_t n >
    void read_register(uint8_t reg, std::array<uint8_t, n> & bytes)
    {
        auto trans = spi.transaction(csn); 

        trans.write( R_REGISTER | reg);
        trans.read(bytes);
    }

    /// \brief
    /// write multiple bytes
    /// \details
    /// This function writes n bytes to one of the registers of the nRF24L01+
    template< size_t n >
    void write_register(uint8_t reg, std::array<uint8_t, n> & bytes)
    {
        auto trans = spi.transaction(csn);

        trans.write(W_REGISTER | (0x20 | reg));
        for(unsigned int i = 0; i < bytes.size(); i++)
        {
            trans.write(bytes[i]);
        }
    }

    /// \brief
    /// set the transmit adress
    /// \details
    /// This function writes the given adress to the registers so the nRF24L01+
    /// can communicate with another nRF24L01+
    template< size_t n >                            
    void write_adress_TX(std::array<uint8_t, n> & adress)
    {
        write_register(TX_ADDR,     adress);
        write_register(RX_ADDR_P0,  adress);
        write_register(RX_PW_P0,    data_size);
    } 

    /// \brief
    /// set the receive adress
    /// \details
    /// This funtion enables the given pipe and writes the give adress to the
    /// right registers so the nRF24L01+ can listen to the transmitted data on the right pipe 
    template< size_t n >
    void write_adress_RX(uint8_t pipe, std::array<uint8_t, n> & adress)
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

    /// \brief
    /// write payload (without ack)
    /// \details
    /// This funtion writes the data to another nRF24L01+ module without
    /// waiting for an acknowledge from the receiver
    template< size_t n >
    void write_noack_payload(std::array<uint8_t, n> & payload)
    {
        auto trans = spi.transaction(csn);

        trans.write( W_TX_NO_ACK );
        for(unsigned int i = 0; i < payload.size(); i++ )
        {
            trans.write(payload[i]);
        }
    }

    /// \brief
    /// read incoming data
    /// \details
    /// This function reads the data from the RX_PLD (arrived data on the RX FIFO)
    /// and saves it in the given array for further use
    template< size_t n>
    void read_payload(std::array<uint8_t, n> & payload)
    {
        auto trans = spi.transaction(csn);

        trans.write( R_RX_PAYLOAD );
        trans.read( payload );
    }

    /// \brief
    /// read incoming data
    /// \details
    /// reads the incoming data and returns the first byte
    template<size_t n>
    bool read(std::array<uint8_t, n> & payload )
    {   
        read_payload( payload ); 
    }
};



#endif // NRF24_HPP