#ifndef OLED_DISPLAY_SHARE_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

//pre-condition: all microcontrollers that share display must have unique adresses
//pre-condition: only one of the microcontrollers may init ownership during initialization

//used when transmitting non oled-display related data via i2c
//prefixing message with this token will make it get forwarded to EXThandler
//as of current version, you cannot request data with this external command
const uint8_t EXTcmd = 99;

// Initialize display share
// address: the adress to give to this microcontroller
// initOwnership: when this is 1, this microcontroller will claim
//                initial ownership of the oled display
// EXThandler: function pointer for handling external commands (EXT)
//             NULL if no external command handler is used
// returns: 0 on successful initialization, -1 if an error occured
//          during initialization
int oledDisplayShareInit(uint8_t address, uint8_t initOwnerShip, void (*EXThandler)(int));

// Check if this microcontroller currently has ownership of display
// returns: true if microcontroller currently has display,
//          else false
int oledDisplayShareHasDisplay();

// Clears display
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareClear();

// Writes a pixel to the display buffer
// X: x coordinate of top-left of pixel
// Y: y coordinate of top-left of pixel
// scale: scale of pixel, pixel display size will be multiplied by this number
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareSetPixel(uint8_t X, uint8_t Y, uint8_t scale);

// Writes a non-filled rectangle to the display buffer
// X: x coordinate of top-left of rectangle
// Y: y coordinate of top-left of rectangle
// width: width in pixels of rectangle
// height: height in pixels of rectangle
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareSetRect(uint8_t X, uint8_t Y, uint8_t width, uint8_t height);

// Writes a character to the display buffer
// X: x coordinate of top-left of character
// Y: y coordinate of top-left of character
// c: the character to display
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareSetChar(uint8_t X, uint8_t Y, uint8_t c);

// Inverts the background and foreground colors in the display buffer
// inv: if this is 1, the back and foreground colors will be inverted
//      if this is 0, they will be set back to normal
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareInvert(uint8_t inv);

// Draws the current display buffer to the actual display
// returns: 0 on success, -1 if microcontroller
//          currently does not have display
int oledDisplayShareDraw();

// Transfers ownership of the display to the microcontroller with the
// specified address
// address: the address of the microcontroller to transfer display
//          ownership to
// returns: 0 on success, -1 if microcontroller currently does not
//          have display or did not get an acknowledgment from other
//          microcontroller during transfer
int oledDisplayShareTransferTo(uint8_t address);

// Requests ownership of the display from the given microcontroller with
// the specified address
// address: the address of the microcontroller to request display
//          ownership of
// returns: 0 on success, -1 if microcontroller currently
//          has display or did not get an acknowledgment from other
//          microcontroller during transfer
int oledDisplayShareRequestFrom(uint8_t address);

#ifdef __cplusplus
}
#endif

#endif
