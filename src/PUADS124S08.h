#ifndef PUADS124S08_H
#define PUADS124S08_H

#include <Arduino.h>
#include <SPI.h>

// #define DEFAULT_SPI_SPEED 1000000 // 1 MHz  For testing
#define DEFAULT_SPI_SPEED 10000000 // 10 MHz (Maximum recommended [100ns period])
#define DEFAULT_SPI_DATA_ORDER MSBFIRST // Datasheet pg. 70
#define DEFAULT_SPI_DATA_MODE SPI_MODE1 // Datasheet pg. 86

#define CH_AINCOM 0b1100 // 12

class PUADS124S08
{
  private:
  int cs; //!< Chip Select pin
  SPISettings spisett = SPISettings(DEFAULT_SPI_SPEED, DEFAULT_SPI_DATA_ORDER, DEFAULT_SPI_DATA_MODE); //!< SPI Settings

  public:

  /**
   * Default constructor (empty)
   */
  PUADS124S08();
  /**
   * Constructor
   * @param cs Chip Select pin
   */
  PUADS124S08(int cs);

  /**
   * Valid commands
   */
  enum Commands {
    RESET = 0x06, //!< Reset
    START = 0x8,
    STOP = 0x0A,
    RDATA = 0x12,
    RREG = 0b00100000, //!< Read register 001r rrrr at adress r rrrr
    WREG = 0b01000000, //!< Write register 010r rrrr at adress r rrrr
  };

  /**
   * Valid registers
   */
  enum Registers {
    STATUS = 0x01, //!< FL_POR | !RDY | FL_P_RAILP | FL_P_RAILN | FL_N_RAILP | FL_N_RAILN | FL_REF_L1 | FL_REF_L0
    INPMUX = 0x02, //!< 7:4 MUXP[3:0] | 3:0 MUXN[3:0]
    REF = 0x05 //!< 7:6 FL_REF_EN[1:0] | !REFP_BUF | !REFN_BUF | 3:2 REFSEL[1:0] | 1:0 REFCON[1:0]
  };

  /**
   * Reset / Initialize ADC
   * @returns true if successful
   */
  bool reset();

  /**
   * Check if ADC is ready
   * @returns true if ready
   */
  bool isready();

  /**
   * Select SPI device
   * 
   * No other device can use SPI while selected
   */
  void select();
  /**
   * Deselect SPI device
   * 
   * Waits >~20ns after deselecting
   */
  void deselect();

  /**
   * Read byte from register at adress adrr
   * @param adrr Adress (5 bits)
   * @returns byte read
   */
  uint8_t readb(uint8_t adrr);

  #ifdef DEBUG
  struct allregs
  {
    uint8_t r0;
    uint8_t r1;
    uint8_t r2;
    uint8_t r3;
    uint8_t r4;
    uint8_t r5;
    uint8_t r6;
    uint8_t r7;
    uint8_t r8;
    uint8_t r9;
    uint8_t r10;
    uint8_t r11;
    uint8_t r12;
    uint8_t r13;
    uint8_t r14;
    uint8_t r15;
    uint8_t r16;
    uint8_t r17;
  };

  /**
   * Read all registers
   * @returns a struct with the content of all registers
   */
  struct allregs readall();
  #endif

  /**
   * Write byte to adress
   * @param adrr The adress to write
   * @param byte The byte to write
   */
  void writeb(uint8_t adrr, uint8_t byte);

  /**
   * Write a bit to a register
   * @param adrr The adress of the register with the bit
   * @param val The bit value
   * @param nbit The bit number 0-7
   */
  void writebit(uint8_t adrr, bool val, unsigned int nbit);

  /**
   * Select positive channel
   * @param n channel number
   */
  void selpchannel(int n);

  /**
   * Select positive channel
   * @param n channel number
   */
  void selnchannel(int n);

  /**
   * Start conversions
   */
  void start();

  /**
   * Stop conversions
   */
  void stop();

  /**
   * Get value from the actual channel (signed)
   */
  int getv();
};

#endif