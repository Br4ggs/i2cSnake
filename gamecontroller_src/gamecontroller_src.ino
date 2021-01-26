//we'll use the provided sparkfun libraries for now.
#include <SparkFun_Qwiic_Joystick_Arduino_Library.h>
#include <Wire.h>

#include "snakeGame.h"
#include "oledDisplayShare.h"

typedef enum scene
{
  STARTSCREEN,
  GAMESCREEN,
  HISCORESCREEN
} Scene;

uint8_t joystickAddress = 0x20;
JOYSTICK joystick;

//todo fix snake game screen wrapping
//todo fix weird visual glitch when colliding with top boundary

const uint8_t address = 5;
const uint8_t hiscoreAddress = 4;

const uint8_t scaler = 4;
const uint8_t displayWidth = 64; // x axis?
const uint8_t displayHeight = 48; //y axis?

Scene currentScene = STARTSCREEN;

void setup() {
  Serial.begin(9600);

  if(joystick.begin() == false)
  {
    Serial.println("joystick is not connected. please connect joystick and restart");
    while(1);
  }

  if(oledDisplayShareInit(5, 1, NULL) != 0)
  {
    Serial.println("something went wrong initializing oled display share. blocking");
    while(1);
  }

  gotoStartScreen();
}

void loop() {
  switch(currentScene)
  {
    case STARTSCREEN:
      handleStartScreenTick();
      break;
    
    case GAMESCREEN:
      handleGameScreenTick();
      break;

    case HISCORESCREEN:
      handleHiscoreScreenTick();
      break;
  }
}

void gotoStartScreen()
{
  currentScene = STARTSCREEN;
  oledDisplayShareClear();
  
  oledDisplayShareSetChar(0, 0, 'S');
  oledDisplayShareSetChar(8, 0, 'N');
  oledDisplayShareSetChar(16, 0, 'A');
  oledDisplayShareSetChar(24, 0, 'K');
  oledDisplayShareSetChar(32, 0, 'E');

  oledDisplayShareSetChar(0, 8, 'G');
  oledDisplayShareSetChar(8, 8, 'A');
  oledDisplayShareSetChar(16, 8, 'M');
  oledDisplayShareSetChar(24, 8, 'E');

  oledDisplayShareSetChar(0, 24, 'P');
  oledDisplayShareSetChar(8, 24, 'R');
  oledDisplayShareSetChar(16, 24, 'E');
  oledDisplayShareSetChar(24, 24, 'S');
  oledDisplayShareSetChar(32, 24, 'S');

  oledDisplayShareSetChar(0, 32, 'B');
  oledDisplayShareSetChar(8, 32, 'T');
  oledDisplayShareSetChar(16, 32, 'N');

  oledDisplayShareSetChar(32, 32, 'T');
  oledDisplayShareSetChar(40, 32, 'O');

  oledDisplayShareSetChar(0, 40, 'P');
  oledDisplayShareSetChar(8, 40, 'L');
  oledDisplayShareSetChar(16, 40, 'A');
  oledDisplayShareSetChar(24, 40, 'Y');
  
  oledDisplayShareDraw();
}

void gotoGameScreen()
{
  currentScene = GAMESCREEN;
  snakeGameInit((displayWidth/scaler), (displayHeight/scaler), 0);
  
  drawGame();
}

void gotoHiscoreScreen()
{
  currentScene = HISCORESCREEN;
  uint16_t score = getSnakeLenght();

  Serial.println(score, BIN);
  
  Wire.beginTransmission(hiscoreAddress);
  Wire.write(EXTcmd);
  Wire.write((score >> 8));
  Wire.write(score);
  Wire.endTransmission();
  
  if(oledDisplayShareTransferTo(hiscoreAddress) == 0)
  {
    Serial.println("transfer successfull");
    delay(500);
  }
  else
  {
    Serial.println("transfer unsuccessfull");
    handleError();
  }
}

void handleStartScreenTick()
{
  if(!joystick.getButton())
  {
    gotoGameScreen();
  }
}

void handleGameScreenTick()
{
  delay(100);
  
  int X = joystick.getHorizontal();
  int Y = joystick.getVertical();
  int B = joystick.getButton();

  SnakeDirection dir = getSnakeDirection();

  if  (X > 575 && canSnakeMoveInDirection(Direction_Right))
  {
    dir = Direction_Right;
  }
  else if (X < 450 && canSnakeMoveInDirection(Direction_Left))
  {
    dir = Direction_Left;
  }

  else if  (Y > 575 && canSnakeMoveInDirection(Direction_Down))
  {
    dir = Direction_Down;
  }
  else if (Y < 450 && canSnakeMoveInDirection(Direction_Up))
  {
    dir = Direction_Up;
  }

  if(moveSnake(dir) == -2)
  {
    drawGameOver();
    gotoHiscoreScreen();
  }
  else
  {
    drawGame();
  }
}

void handleHiscoreScreenTick()
{
  if(!joystick.getButton())
  {
    if(oledDisplayShareRequestFrom(hiscoreAddress) == 0)
    {
      Serial.println("request successfull");
      delay(500);
      gotoStartScreen();
    }
    else
    {
      Serial.println("request unsuccessfull");
      handleError();
    }
  }
}

void drawGame()
{
  oledDisplayShareClear();

  uint16_t snakeLenght = getSnakeLenght();
  GameObject snakePart;
  getSnakePart(0, &snakePart);

  oledDisplayShareSetRect(0, 0, displayWidth, displayHeight);

  for(int i = 0; i < snakeLenght; i++)
  {
    GameObject snakePart;
    getSnakePart(i, &snakePart);

    oledDisplayShareSetPixel(snakePart.X, snakePart.Y, scaler);
  }

  GameObject snakeFood = getSnakeFood();
  oledDisplayShareSetPixel(snakeFood.X, snakeFood.Y, scaler);

  oledDisplayShareDraw();
}

void drawGameOver()
{
  uint8_t inv = 1;
  for(int i = 0; i < 6; i++)
  {
    oledDisplayShareInvert(inv);
    drawGame();

    inv = !inv;
    
    delay(50);
  }
}

void handleError()
{
  Serial.println("ERROR");

  while(1)
  {
    oledDisplayShareClear();
    oledDisplayShareSetChar(0, 0, 'E');
    oledDisplayShareSetChar(8, 0, 'R');
    oledDisplayShareSetChar(16, 0, 'R');
    oledDisplayShareSetChar(24, 0, 'O');
    oledDisplayShareSetChar(32, 0, 'R');
    oledDisplayShareDraw();
    delay(500);
  }
}
