#include <SFE_MicroOLED.h>
#include <Wire.h>

#include "oledDisplayShare.h"

typedef enum state
{
  WAITING_FOR_DISPLAY,
  HAS_DISPLAY
} State;

uint16_t highScores[3] = {0};

State currentState = WAITING_FOR_DISPLAY;

void setup()
{
  Serial.begin(9600);

  if(oledDisplayShareInit(4, 0, EXThandler) != 0)
  {
    Serial.println("something went wrong initializing oled display share. blocking");
    while(1);
  }
  
  gotoIdleState();
}

void loop()
{
  switch(currentState)
  {
    case WAITING_FOR_DISPLAY:
      handleIdleTick();
      break;
    case HAS_DISPLAY:
      handleDisplayTick();
      break;
  }
}

void gotoIdleState()
{
  currentState = WAITING_FOR_DISPLAY;
}

void gotoDisplayState()
{
  currentState = HAS_DISPLAY;
  displayHighScores();
}

void handleIdleTick()
{
  if(oledDisplayShareHasDisplay())
  {
    gotoDisplayState();
  }
}

void handleDisplayTick()
{
  if(!oledDisplayShareHasDisplay())
  {
    gotoIdleState();
  }
}

void displayHighScores()
{
    oledDisplayShareClear();

    oledDisplayShareSetChar(0, 0, 'H');
    oledDisplayShareSetChar(8, 0, 'I');
    oledDisplayShareSetChar(16, 0, 'G');
    oledDisplayShareSetChar(24, 0, 'H');

    oledDisplayShareSetChar(0, 8, 'S');
    oledDisplayShareSetChar(8, 8, 'C');
    oledDisplayShareSetChar(16, 8, 'O');
    oledDisplayShareSetChar(24, 8, 'R');
    oledDisplayShareSetChar(32, 8, 'E');

    oledDisplayShareSetChar(0, 16, '1');
    oledDisplayShareSetChar(8, 16, ')');
    printScore(16, 16, highScores[0]);

    oledDisplayShareSetChar(0, 24, '2');
    oledDisplayShareSetChar(8, 24, ')');
    printScore(16, 24, highScores[1]);

    oledDisplayShareSetChar(0, 32, '3');
    oledDisplayShareSetChar(8, 32, ')');
    printScore(16, 32, highScores[2]);

    oledDisplayShareDraw();
}

void printScore(uint8_t startCol, uint8_t row, uint16_t score)
{
    String scoreStr = String(score);
    Serial.println(score);
    Serial.println(scoreStr);
    for(int i = 0; i < scoreStr.length(); i++)
    {
      oledDisplayShareSetChar(startCol + (8 * i), row, scoreStr.charAt(i));
    }
}

void EXThandler(int howMany)
{
  uint16_t currentScore = 0;
  currentScore |= ((uint16_t)Wire.read() << 8);
  currentScore |= (uint16_t)Wire.read();

  for(int i = 0; i < 3; i++)
  {
    if(currentScore > highScores[i])
    {
      uint16_t tmpScore = highScores[i];
      highScores[i] = currentScore;
      currentScore = tmpScore;
    }
  }
}
