#include "snakeGame.h"
#include "stdlib.h"

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

static uint8_t    fieldWidth_ = 0;
static uint8_t    fieldHeight_ = 0;
static uint8_t    wrapField_ = 0;
static uint16_t   snakeLenght_ = 0;
static GameObject snakeFood_;
static GameObject* snakeBody_;
static SnakeDirection snakeHeadDirection_;

static int checkIfSnakeOutOfBounds();
static int checkIfSnakeAteFood();
static int checkIfSnakeCollided();
static void placeSnakeFood();
static void extendSnake();
static void wrapSnakeHead();

int snakeGameInit(uint8_t fieldWidth, uint8_t fieldHeight, uint8_t wrapField)
{ 
  if(snakeLenght_ != 0)
  {
    free(snakeBody_);
  }
  
  fieldWidth_ = fieldWidth;
  fieldHeight_ = fieldHeight;
  wrapField_ = wrapField;
  snakeLenght_ = 3;

  uint16_t arraySize = (fieldWidth_ * fieldHeight_) - 1;

  snakeBody_ = (GameObject*)malloc(sizeof(GameObject) * arraySize);

  if(snakeBody_ == NULL)
  {
    return -1;
  }

  //todo place initial snake in the top-middle of the screen
  snakeBody_[0].X = 4;
  snakeBody_[0].Y = 4;
  snakeBody_[1].X = 4;
  snakeBody_[1].Y = 3;
  snakeBody_[2].X = 4;
  snakeBody_[2].Y = 2;

  snakeHeadDirection_ = Direction_Down;

  placeSnakeFood();

  return 0;
}

uint16_t getSnakeLenght()
{
  return snakeLenght_;
}

SnakeDirection getSnakeDirection()
{
  return snakeHeadDirection_;
}

int getSnakePart(uint16_t index, GameObject* snakePart)
{
  if(index >= snakeLenght_)
  {
    return -1;
  }
  
  *snakePart = snakeBody_[index];

  return 0;
}

GameObject getSnakeFood()
{
  return snakeFood_;
}

int moveSnake(SnakeDirection dir)
{
  if(!canSnakeMoveInDirection(dir))
  {
    return -1;
  }

  if(checkIfSnakeAteFood())
  {
    extendSnake();
    placeSnakeFood();
  }

  snakeHeadDirection_ = dir;
  GameObject prevSegment = snakeBody_[0];
  switch(dir)
  {
    case Direction_Up:
    {
      snakeBody_[0].Y--;
    }
    break;
    case Direction_Down:
    {
      snakeBody_[0].Y++;
    }
    break;
    case Direction_Left:
    {
      snakeBody_[0].X--;
    }
    break;
    case Direction_Right:
    {
      snakeBody_[0].X++;
    }
    break;
  }

  if(wrapField_ && checkIfSnakeOutOfBounds())
  {
    wrapSnakeHead(); //todo fix this function
  }

  for(int i = 1; i < snakeLenght_; i++)
  {
    GameObject currentSegment = snakeBody_[i];
    snakeBody_[i] = prevSegment;
    prevSegment = currentSegment;
  }

  return (checkIfSnakeCollided()) ? -2 : 0;
}

int canSnakeMoveInDirection(SnakeDirection dir)
{
  switch(dir)
  {
    case Direction_Up:
    {
      return (snakeHeadDirection_ != Direction_Down);
    }
    break;
    case Direction_Down:
    {
      return (snakeHeadDirection_ != Direction_Up);
    }
    break;
    case Direction_Left:
    {
      return (snakeHeadDirection_ != Direction_Right);
    }
    break;
    case Direction_Right:
    {
      return (snakeHeadDirection_ != Direction_Left);
    }
    break;
  }
}

static int checkIfSnakeOutOfBounds()
{
  GameObject snakeHead = snakeBody_[0];

  return (snakeHead.X >= fieldWidth_) || (snakeHead.Y >= fieldHeight_);
}

static int checkIfSnakeAteFood()
{
  GameObject snakeHead = snakeBody_[0];
  
  return (snakeHead.X == snakeFood_.X && snakeHead.Y == snakeFood_.Y);
}

static int checkIfSnakeCollided()
{
  GameObject snakeHead = snakeBody_[0];

  for(int i = 1; i < snakeLenght_; i++)
  {
    if(snakeHead.X == snakeBody_[i].X && snakeHead.Y == snakeBody_[i].Y)
    {
      return 1;
    }
  }

  if(!wrapField_ && checkIfSnakeOutOfBounds())
  {
    return 1;
  }

  return 0;
}

static void placeSnakeFood()
{
  snakeFood_.X = rand() % fieldWidth_;
  snakeFood_.Y = rand() % fieldHeight_;
}

static void extendSnake()
{
  GameObject prevTail;
  getSnakePart(snakeLenght_, &prevTail);
  snakeLenght_++;
  snakeBody_[snakeLenght_].X = prevTail.X;
  snakeBody_[snakeLenght_].Y = prevTail.Y;
}

static void wrapSnakeHead() //todo fix this
{
  GameObject snakeHead = snakeBody_[0];

  if(snakeHead.X == fieldWidth_ - 1)
  {
    snakeHead.X = 0;
  }
  else if(snakeHead.X == 0)
  {
    snakeHead.X = fieldWidth_ - 1;
  }
  else if(snakeHead.Y == fieldHeight_ -1)
  {
    snakeHead.Y = 0;
  }
  else if(snakeHead.Y == 0)
  {
    snakeHead.Y = fieldHeight_ - 1;
  }
}

#ifdef __cplusplus
}
#endif
