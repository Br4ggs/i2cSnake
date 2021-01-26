#include "oledDisplayShare.h"
#include "stdlib.h"

#include <SFE_MicroOLED.h>
#include <Wire.h>
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ownershipCmd
{
  OWNERSHIP_TRANSFER = 10,
  OWNERSHIP_REQUEST = 11
} OwnershipCmd;

typedef enum ownershipRsp
{
  NONE = 20,
  TRANSFER_ACK = 21,
  TRANSFER_NACK = 22,
  REQUEST_ACK = 23,
  REQUEST_NACK = 24
} OwnershipRsp;

static uint8_t resetPin = 9;
static uint8_t dcJumper = 1;
static MicroOLED oled(resetPin, dcJumper);

static uint8_t hasDisplay_ = 0;

static OwnershipRsp ownershipRsp_ = NONE;

static void (*EXThandler_)(int) = NULL; 

static void receiveEvent(int howMany);
static void requestEvent();

int oledDisplayShareInit(uint8_t address, uint8_t initOwnerShip, void (*EXThandler)(int))
{
  EXThandler_ = EXThandler;
  
  Wire.begin(address);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  oled.begin();

  if(initOwnerShip)
  {
    hasDisplay_ = 1;

    oled.clear(ALL);
  }
  else
  {
    hasDisplay_ = 0;
  }

  return 0;
}

int oledDisplayShareHasDisplay()
{
  return hasDisplay_;
}

int oledDisplayShareClear()
{
  if(!hasDisplay_)
  {
    return -1;
  }

  oled.clear(PAGE);

  return 0;
}

int oledDisplayShareSetPixel(uint8_t X, uint8_t Y, uint8_t scale)
{
  if(!hasDisplay_)
  {
    return -1;
  }
  
  oled.rectFill(X * scale, Y * scale, scale, scale);
}

int oledDisplayShareSetRect(uint8_t X, uint8_t Y, uint8_t width, uint8_t height)
{
  if(!hasDisplay_)
  {
    return -1;
  }

  oled.rect(X, Y, width, height);
}

int oledDisplayShareSetChar(uint8_t X, uint8_t Y, uint8_t c)
{
  if(!hasDisplay_)
  {
    return -1;
  }
  
  oled.drawChar(X, Y, c); 
}

int oledDisplayShareInvert(uint8_t inv)
{
  if(!hasDisplay_)
  {
    return -1;
  }

  oled.invert(inv);
}

int oledDisplayShareDraw()
{
  if(!hasDisplay_)
  {
    return -1;
  }
  
  oled.display();
}

int oledDisplayShareTransferTo(uint8_t address)
{
  if(!hasDisplay_)
  {
    return -1;
  }
  
  Wire.beginTransmission(address);
  Wire.write(OWNERSHIP_TRANSFER);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);

  uint8_t counter = 0;
  while(!Wire.available())
  {
    delayMicroseconds(1);
    counter++;

    if(counter >= 250)
    {
      return -1;
    }
  }

  OwnershipRsp rsp = (OwnershipRsp)Wire.read();
  if(rsp != TRANSFER_ACK)
  {
    return -1;
  }
  
  hasDisplay_ = 0;
  return 0;
}

int oledDisplayShareRequestFrom(uint8_t address)
{
  if(hasDisplay_)
  {
    return -1;
  }

  Wire.beginTransmission(address);
  Wire.write(OWNERSHIP_REQUEST);
  Wire.endTransmission();
  
  Wire.requestFrom(address, 1);

  uint8_t counter = 0;
  while(!Wire.available())
  {
    delayMicroseconds(1);
    counter++;

    if(counter >= 250)
    {
      return -1;
    }
  }

  OwnershipRsp rsp = (OwnershipRsp)Wire.read();
  if(rsp != REQUEST_ACK)
  {
    return -1;
  }
  
  hasDisplay_ = 1;
  return 0;
}

static void receiveEvent(int howMany)
{
  OwnershipCmd cmd = (OwnershipCmd)Wire.read();

  switch(cmd)
  {
    case OWNERSHIP_TRANSFER:
    {
      if(!hasDisplay_)
      {
        ownershipRsp_ = TRANSFER_ACK;
        hasDisplay_ = 1;
      }
      else
      {
        ownershipRsp_ = TRANSFER_NACK;
      }

      break;
    }

    case OWNERSHIP_REQUEST:
    {
      if(hasDisplay_)
      {
        ownershipRsp_ = REQUEST_ACK;
        hasDisplay_ = 0;
      }
      else
      {
        ownershipRsp_ = REQUEST_NACK;
      }

      break;
    }
    case EXTcmd:
    {
      if(EXThandler_ != NULL)
      {
        EXThandler_(howMany - 1);
      }
      break;
    }
  }
}

static void requestEvent()
{
  Wire.write(ownershipRsp_);
  ownershipRsp_ = NONE;
}

#ifdef __cplusplus
}
#endif
