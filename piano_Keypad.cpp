#include "piano_Keypad.h"

//int dataPin = 2;  // pin D7 on NodeMCU /SSD1
//int clockPin = 4; // pin D5 on NodeMCU boards //sdclk
//int latchPin = 3; // pin D8 on NodeMCU boards //sdcmd


// Out Shifting Register
#define SOR_dataPin  D5  // Pin 3 D7 on 74HC595
#define SOR_clockPin D7  // Pin 6 D5 on 74HC595
#define SOR_latchPin D6  // Pin 5 D8 on 74HC595

// In Shifting Register
#define SIR_dataPin  D4  // Pin 8 on 74HC595
#define SIR_clockPin D3  // Pin 10 on 74HC595
#define SIR_latchPin D2  // Pin 9 on 74HC595

//ShiftRegister<1>sr( dataPin, clockPin, latchPin);
ShiftInRegister<1> sir(SIR_dataPin, SIR_clockPin, SIR_latchPin);
ShiftOutRegister<1> sor(SOR_dataPin, SOR_clockPin, SOR_latchPin);

#define _KEY_PRESSED_POS (1)
#define _KEY_PRESSED (1UL << _KEY_PRESSED_POS)

#define _JUST_PRESSED_POS (2)
#define _JUST_PRESSED (1UL << _JUST_PRESSED_POS)

#define _JUST_RELEASED_POS (3)
#define _JUST_RELEASED (1UL << _JUST_RELEASED_POS)

#define _KEYPAD_SETTLING_DELAY 20

int bits[] = { B11111110, // 7
               B11111101, // 6
               B11111011, // 5 
               B11110111, // 4
               B11101111, // 3
               B11011111, // 2
               B10111111, // 1
               B01111111  // 1
  };
  
/**************************************************************************/
/*!
    @brief  default constructor
    @param  userKeymap a multidimensional array of key characters
    @param  row an array of GPIO pins that are connected to each row of the
   keypad
    @param  col an array of GPIO pins that are connected to each column of the
   keypad
    @param  numRows the number of rows on the keypad
    @param  numCols the number of columns on the keypad
*/
/**************************************************************************/
Piano_Keypad::Piano_Keypad(byte *userKeymap, byte *row, byte *col,
                                 int numRows, int numCols) {
  _userKeymap = userKeymap;
  _col = col;
  _row = row;
  _numCols = numCols;
  _numRows = numRows;

  _keystates = NULL;
}

/**************************************************************************/
/*!
    @brief  default destructor
*/
/**************************************************************************/
Piano_Keypad::~Piano_Keypad() {
  if (_keystates != NULL) {
    free((void *)_keystates);
  }
}

/**************************************************************************/
/*!
    @brief  get the state of a key with the given name
    @param  key the name of the key to be checked
*/
/**************************************************************************/
volatile byte *Piano_Keypad::getKeyState(byte key) {
  for (int i = 0; i < _numCols * _numRows ; i++) {
    if (_userKeymap[i] == key) {
      return _keystates + i;
    }
  }
  return NULL;
}

/**************************************************************************/
/*!
    @brief  read the array of switches and place any events in the buffer.
*/
/**************************************************************************/
void Piano_Keypad::tick() {
  uint8_t evt;
  for (int i = 0; i < _numRows; i++) {
     //sor.set(_row[i], HIGH);
  }

  int i = 0;
  for (int r = 0; r < _numRows; r++) {
    // sor.set(_row[r], LOW);
    sor.scanRows(bits[r]);
    delayMicroseconds(_KEYPAD_SETTLING_DELAY);
    for (int c = 0; c < _numCols; c++) {
      i = c * _numRows + r;
      bool pressed = sir.get(_col[c]);
      volatile byte *state = _keystates + i;
      byte currentState = *state;
      if (pressed && !(currentState & _KEY_PRESSED)) {
        currentState |= (_JUST_PRESSED | _KEY_PRESSED);
        evt = KEY_JUST_PRESSED;
        _eventbuf.store_char(evt);
        _eventbuf.store_char(*(_userKeymap + i));
      } else if (!pressed && (currentState & _KEY_PRESSED)) {
        currentState |= _JUST_RELEASED;
        currentState &= ~(_KEY_PRESSED);
        evt = KEY_JUST_RELEASED;
        _eventbuf.store_char(evt);
        _eventbuf.store_char(*(_userKeymap + i));
      }
      *state = currentState;
    }
    // Serial.println("");
   //  sor.set(_row[r], HIGH);
  }
}

/**************************************************************************/
/*!
    @brief  set all the pin modes and set up variables.
*/
/**************************************************************************/
void Piano_Keypad::begin() {
  _keystates = (volatile byte *)malloc(_numCols * _numRows);
  memset((void *)_keystates, 0, _numCols * _numRows);

  for (int i = 0; i < _numRows; i++) {
    // sor.set(_row[i], HIGH);
  }

  for (int i = 0; i < _numCols; i++) {
   // pinMode(_col[i], INPUT_PULLUP);
  }
}

/**************************************************************************/
/*!
    @brief  check if the given key has just been pressed since the last tick.
    @param  key the name of the key to be checked
    @param  clear whether to reset the state (default yes) post-check
    @returns    true if it has been pressed, false otherwise.
*/
/**************************************************************************/
bool Piano_Keypad::justPressed(byte key, bool clear) {
  volatile byte *state = getKeyState(key);
  bool val = (*state & _JUST_PRESSED) != 0;

  if (clear)
    *state &= ~(_JUST_PRESSED);

  return val;
}

/**************************************************************************/
/*!
    @brief  check if the given key has just been released since the last tick.
    @param  key the name of the key to be checked
    @returns    true if it has been released, false otherwise.
*/
/**************************************************************************/
bool Piano_Keypad::justReleased(byte key) {
  volatile byte *state = getKeyState(key);
  bool val = (*state & _JUST_RELEASED) != 0;

  *state &= ~(_JUST_RELEASED);

  return val;
}

/**************************************************************************/
/*!
    @brief  check if the given key is currently pressed
    @param  key the name of the key to be checked
    @returns    true if it is currently pressed, false otherwise.
*/
/**************************************************************************/
bool Piano_Keypad::isPressed(byte key) {
  return (*getKeyState(key) & _KEY_PRESSED) != 0;
}

/**************************************************************************/
/*!
    @brief  check if the given key is currently released
    @param  key the name of the key to be checked
    @returns    true if it is currently released, false otherwise.
*/
/**************************************************************************/
bool Piano_Keypad::isReleased(byte key) {
  return (*getKeyState(key) & _KEY_PRESSED) == 0;
}

/**************************************************************************/
/*!
    @brief  check how many events are in the keypads buffer
    @returns    the number of events currently in the buffer
*/
/**************************************************************************/
int Piano_Keypad::available() { return (_eventbuf.available() >> 1); }

/**************************************************************************/
/*!
    @brief  pop the next event off of the FIFO
    @returns    the next event in the FIFO
*/
/**************************************************************************/
keypadEvent Piano_Keypad::read() {
  keypadEvent k;
  k.bit.EVENT = _eventbuf.read_char();
  k.bit.KEY = _eventbuf.read_char();

  return k;
}

/**************************************************************************/
/*!
    @brief Clear out the event buffer and all the key states
*/
/**************************************************************************/
void Piano_Keypad::clear() {
  _eventbuf.clear();
  for (int i = 0; i < _numCols * _numRows; i++)
    *(_keystates + i) = 0;
}
