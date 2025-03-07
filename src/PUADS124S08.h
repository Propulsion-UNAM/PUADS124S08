#ifndef PUADS124S08_H
#define PUADS124S08_H

#include <Arduino.h>
#include <SPI.h>

#define DEFAULT_SPI_SPEED 100000000 // 100 MHz
#define DEFAULT_SPI_DATA_ORDER MSBFIRST // Datasheet pg. 70
#define DEFAULT_SPI_DATA_MODE SPI_MODE1 // Datasheet pg. 86

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
    RREG = 0b00100000, //!< Read register 001r rrrr at adress r rrrr
  };

  /**
   * Valid registers
   */
  enum Registers {
    STATUS = 0x01, //!< FL_POR | RDY | FL_P_RAILP | FL_P_RAILN | FL_N_RAILP | FL_N_RAILN | FL_REF_L1 | FL_REF_L0
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
};

#endif