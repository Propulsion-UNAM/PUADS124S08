#include "PUADS124S08.h"

PUADS124S08::PUADS124S08()
{

}

void PUADS124S08::deselect()
{
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
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
  SPI.transfer(Commands::RESET);
  deselect();

  delay(2);
  if (!isready())
  {
    return false;
  }
  writebit(Registers::STATUS, false, 7);
  return true;
}

bool PUADS124S08::isready()
{
  uint8_t byte = readb(Registers::STATUS);

  #ifdef DEBUG
  Serial.println(millis());
  Serial.print("Status register: ");
  Serial.print("0b");
  Serial.print(byte, BIN);
  Serial.println();
  #endif

  return !(byte & 0b01000000);
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

#ifdef DEBUG
PUADS124S08::allregs PUADS124S08::readall()
{
  struct allregs regs;
  select();
  SPI.transfer(Commands::RREG);
  SPI.transfer(0b00010011);
  regs.r0 = SPI.transfer(0x00);
  regs.r1 = SPI.transfer(0x00);
  regs.r2 = SPI.transfer(0x00);
  regs.r3 = SPI.transfer(0x00);
  regs.r4 = SPI.transfer(0x00);
  regs.r5 = SPI.transfer(0x00);
  regs.r6 = SPI.transfer(0x00);
  regs.r7 = SPI.transfer(0x00);
  regs.r8 = SPI.transfer(0x00);
  regs.r9 = SPI.transfer(0x00);
  regs.r10 = SPI.transfer(0x00);
  regs.r11 = SPI.transfer(0x00);
  regs.r12 = SPI.transfer(0x00);
  regs.r13 = SPI.transfer(0x00);
  regs.r14 = SPI.transfer(0x00);
  regs.r15 = SPI.transfer(0x00);
  regs.r16 = SPI.transfer(0x00);
  regs.r17 = SPI.transfer(0x00);
  deselect();
  return regs;
}
#endif

void PUADS124S08::writeb(uint8_t adrr, uint8_t byte)
{
  uint8_t com = Commands::WREG | (adrr & 0b00011111);
  select();
  SPI.transfer(com);
  SPI.transfer(0x00);
  SPI.transfer(byte);
  deselect();
}

void PUADS124S08::writebit(uint8_t adrr, bool val, unsigned int nbit)
{
  if (nbit > 7) return;

  int b;
  if (val)
  {
    b = 0b1;
  } else {
    b = 0b0;
  }

  uint8_t byte = (readb(adrr) & ~(1 << nbit)) | (b << nbit);

  uint8_t com = Commands::WREG | (adrr & 0b00011111);
  select();
  SPI.transfer(com);
  SPI.transfer(0x00);
  SPI.transfer(byte);
  deselect();
}

void PUADS124S08::selpchannel(int n, bool wait)
{
  if (n > 12) return;
  uint8_t nmux = (readb(Registers::INPMUX) & 0b00001111) | (0b11110000 & (n << 4));
  writeb(Registers::INPMUX, nmux);
  if (!wait) return;
  delayMicroseconds(getswitchdelay());
}

void PUADS124S08::selnchannel(int n, bool wait)
{
  if (n > 12) return;
  uint8_t nmux = (readb(Registers::INPMUX) & 0b11110000) | (0b00001111 & n);
  writeb(Registers::INPMUX, nmux);
  if (!wait) return;
  delayMicroseconds(getswitchdelay());
}

void PUADS124S08::start(bool wait)
{
  select();
  SPI.transfer(Commands::START);
  deselect();
  if (!wait) return;
  delayMicroseconds(pgadelay * 4);
}

void PUADS124S08::stop()
{
  select();
  SPI.transfer(Commands::STOP);
  deselect();
}

int PUADS124S08::getv()
{
  int data = 0;
  select();
  SPI.transfer(Commands::RDATA);
  uint8_t b1 = SPI.transfer(0x00);
  uint8_t b2 = SPI.transfer(0x00);
  uint8_t b3 = SPI.transfer(0x00);
  deselect();
  data = (b1 << 16) | (b2 << 8) | b3;

  if (data & 0b100000000000000000000000)
  {
    data = data | 0b11111111000000000000000000000000;
  }

  return data;
}

void PUADS124S08::setdelay(int d)
{
  if (d > 7) return;
  uint8_t gset = (readb(Registers::GAINSET) & 0b00011111) | (0b11100000 & (d << 5));
  writeb(Registers::GAINSET, gset);

  switch (d)
  {
  case Delays::TMOD1:
    pgadelay = 1;
    break;
  case Delays::TMOD14:
    pgadelay = 14;
    break;
  case Delays::TMOD25:
    pgadelay = 25;
    break;
  case Delays::TMOD64:
    pgadelay = 64;
    break;
  case Delays::TMOD256:
    pgadelay = 256;
    break;
  case Delays::TMOD1024:
    pgadelay = 1024;
    break;
  case Delays::TMOD2048:
    pgadelay = 2048;
    break;
  case Delays::TMOD4096:
    pgadelay = 4096;
    break;
  
  default:
    pgadelay = 1;
    break;
  }
}

int PUADS124S08::getswitchdelay()
{
  if (sps >= 4000) {
    return 410;
  } else if (sps >= 2000) {
    return 660;
  } else if (sps >= 1000) {
    return 1200;
  } else if (sps >= 800) {
    return 1500;
  } else if (sps >= 400) {
    return 2700;
  } else if (sps >= 200) {
    return 5200;
  } else if (sps >= 100) {
    return 10200;
  } else if (sps >= 60) {
    return 17000;
  } else if (sps >= 50) {
    return 20200;
  } else if (sps >= 20) {
    return 56600;
  } else if (sps >= 16.6) {
    return 60300;
  } else if (sps >= 10) {
    return 106600;
  } else if (sps >= 5) {
    return 206600;
  } else {
    return 406600;
  }
}

void PUADS124S08::setdatarate(int d)
{
  if (d > 13) return;
  uint8_t drate = (readb(Registers::DATARATE) & 0b11110000) | (0b00001111 & d);
  writeb(Registers::DATARATE, drate);

  switch (d)
  {
  case Datarates::SPS4000:
    sps = 4000;
    break;
  case Datarates::SPS2000:
    sps = 2000;
    break;
  case Datarates::SPS1000:
    sps = 1000;
    break;
  case Datarates::SPS800:
    sps = 800;
    break;
  case Datarates::SPS400:
    sps = 400;
    break;
  case Datarates::SPS200:
    sps = 200;
    break;
  case Datarates::SPS100:
    sps = 100;
    break;
  case Datarates::SPS60:
    sps = 60;
    break;
  case Datarates::SPS50:
    sps = 50;
    break;
  case Datarates::SPS20:
    sps = 20;
    break;
  case Datarates::SPS16_6:
    sps = 16.6;
    break;
  case Datarates::SPS10:
    sps = 10;
    break;
  case Datarates::SPS5:
    sps = 5;
    break;
  case Datarates::SPS2_5:
    sps = 2.5;
    break;
  
  default:
    sps = 4000;
    break;
  }
}