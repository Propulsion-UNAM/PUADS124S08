#include "PUADS124S08.h"

PUADS124S08::PUADS124S08()
{

}

void PUADS124S08::deselect()
{
  digitalWrite(cs, HIGH);
  // Wait >~20ns
  asm (
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
  );
}

void PUADS124S08::select()
{
  SPI.beginTransaction(spisett);
  digitalWrite(cs, LOW);
  // TODO: Wait 4096 * (1 / freq)
  asm (
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
    "nop" "\n"
  );
}

PUADS124S08::PUADS124S08(int cs) : cs(cs)
{
  pinMode(cs, OUTPUT);
  deselect();
}

bool PUADS124S08::reset()
{
  select();
  delay(10);
  SPI.transfer(0x06);
  deselect();

  delayMicroseconds(82);
  if (!isready())
  {
    return false;
  }
  // TODO: FL_POR flag
  return true;
}

bool PUADS124S08::isready()
{
  uint8_t byte = readb(Registers::STATUS);
  return (byte & 0b01000000);
}

uint8_t PUADS124S08::readb(uint8_t adrr)
{
  uint8_t com = Commands::RREG | (adrr & 0b00011111);
  uint8_t r;
  select();
  SPI.transfer(com);
  SPI.transfer(0x00);
  r = SPI.transfer(0x00);
  deselect();
  return r;
}