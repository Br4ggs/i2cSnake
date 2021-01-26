#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

//idea for optimizing storage:
//only store corners of snake and the extention afterwards

typedef enum snakeDirection
{
  Direction_Up,
  Direction_Down,
  Direction_Left,
  Direction_Right
} SnakeDirection;

typedef enum gameState
{
  Game_Ok,
  Game_Over_Collision,
  Game_Over_No_Room_Left
} GameState;

typedef struct gameObject
{
  uint8_t X;
  uint8_t Y;
} GameObject;

// Initializes (or resets) the game with the given parameters
// fieldWidth: the width of the playing field
// fieldHeight: the height of the playing field
// wrapField: if this is true, the snake will wrap around to the other side of the
//            playing area when reaching a wall. if this is false hitting a wall
//            will result in a collision gameover
// returns: -1 if the given playfields dimensions are too big for on-board memory
//          else 0
int snakeGameInit(uint8_t fieldWidth, uint8_t fieldHeight, uint8_t wrapField);

// Get the current length of the snake
// returns: the current total length of the snakes body
uint16_t getSnakeLenght();

// Get the current direction of the snake head
// returns: the current direction of the snake head
SnakeDirection getSnakeDirection();

// Get the snake segment with specified index
// index: the index of the snake segment
// snakePart: pointer to store a copy of the snake segment in
// returns: -
int getSnakePart(uint16_t index, GameObject* snakePart);

// Get the position of the snake food
// returns: a copy of the snake food GameObject
GameObject getSnakeFood();

// Moves snake 1 tile into specified direction
// dir: the direction for the snake to move in
// returns: -1 if an unallowed direction was given
//          -2 if the snake collided with itself or the borders
//          else 0
int moveSnake(SnakeDirection dir);

// Checks if snake can currently move in specified direction
// dir: the direction for the snake to move in
// returns: true if possible, false if not possible
int canSnakeMoveInDirection(SnakeDirection dir);

#ifdef __cplusplus
}
#endif

#endif
