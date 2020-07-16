
//SPI COMMANDS
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define W_TX_NO_ACK   0xB0


//MAIN REGISTERS
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define TX_ADDR     0x10
#define FIFO_STATUS 0x17
#define FEATURES    0x1D

//mode definition
#define PWR_UP      2
#define PRIM_RX     1


//PIPE INFO
#define ERX_P0      0x01
#define ERX_P1      0x02
#define ERX_P2      0x04
#define ERX_P3      0x08
#define ERX_P4      0x10
#define ERX_P5      0x20