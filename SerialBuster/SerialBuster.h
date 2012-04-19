// Non blocking (chunked) Serial read/write

// Supports the use of custom commands and custom callbacks each command

// Protocol structure:
//  START     1byte (uint8)
//  RECIPIENT 1byte (uint8)
//  SENDER    1byte (uint8)
//  LENGTH    2byte (uint16)
//  PAYLOAD   Nbyte
//  CRC8      1byte (uint8)
//  END       1byte (uint8)


#ifndef SerialBuster_h
#define SerialBuster_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Buffer.h"

typedef void (*event_cb_t)(uint8_t recipient, Buffer * payload, uint16_t length);

// for checking if the serial line is busy
#if defined(__AVR_ATmega8__)
  #define SB_UCSRA            UCSRA
  #define SB_UDRE             UDRE
#else
  #define SB_UCSRA            UCSR0A
  #define SB_UDRE             UDRE0
#endif

// protocol constants
#define SB_START              0x02
#define SB_ESC                0x1B
#define SB_END                0x03
#define SB_ESC_END            0x1C
#define SB_ESC_ESC            0x1D
#define SB_ESC_START          0x1E
#define SB_BROADCAST          0xFF
#define SB_MASTER             0x00

#define SB_ENVELOPE_SIZE      7
#define SB_PACKET_HEADER_SIZE 5
#define SB_RS485_TX_GRACETIME 2 //ms

class SerialBuster {
  public:
    SerialBuster(uint16_t in_size, uint16_t out_size, uint16_t max_packet_size);
    ~SerialBuster();
    void init(long baud_rate);
    void setRS485pins(uint8_t tx_enable, uint8_t rx_enable);
    void enableTx(bool tx_enable);
    void setCallback(void (*cb)(uint8_t recipient, Buffer * payload, uint16_t length));
    void setAddress(uint8_t address);
    
    void send();
    void update();  // call in loop
    
    bool isSending();
    bool isReceiving();
    
    uint8_t sendPacket(uint8_t recipient, Buffer * payload, uint16_t length);
    uint8_t sendPacket(uint8_t recipient, const uint8_t * payload, uint16_t length);
    uint8_t crc8(Buffer * data, uint16_t len, uint16_t offset);
    
  protected:
    void appendIncoming(uint8_t incoming);
    
    event_cb_t _cb;
    Buffer* _in_buf;
    Buffer* _out_buf;
    Buffer* _packet_buf;
    uint16_t _address;
    uint8_t _tx_enable_pin;
    uint8_t _rx_enable_pin;
    bool _in_tx_mode;
    uint64_t _tx_timer; // timer for when to disable tx.
};


#endif // SerialBuster_h