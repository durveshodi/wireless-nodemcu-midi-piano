#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

#include <Arduino.h>

/*------------------------------------------- Out Shifting Register -------------------------------------------*/
template<uint8_t Size>
class ShiftOutRegister 
{
public:
    ShiftOutRegister(const uint8_t serialDataPin, const uint8_t clockPin, const uint8_t latchPin);
    
    void setAll(const uint8_t * digitalValues);
#ifdef __AVR__
    void setAll_P(const uint8_t * digitalValuesProgmem); // Experimental, PROGMEM data
#endif
    void set(const uint8_t pin, const uint8_t value);
    void setNoUpdate(const uint8_t pin, uint8_t value);
    void updateOutRegisters();
    void setAllLow();
    void setAllHigh(); 
    void scanRows(int value);
private:
    uint8_t _clockPin;
    uint8_t _serialDataPin;
    uint8_t _latchPin;
    uint8_t  _digitalValues[Size];
};

/*------------------------------------------- In Shifting Register --------------------------------------------*/
template<uint8_t Size1>
class ShiftInRegister 
{
public:
    ShiftInRegister(const uint8_t serialDataPin, const uint8_t clockPin, const uint8_t latchPin);
    void updateInRegisters(); 
    bool get(uint16_t n);
    bool get(uint8_t _Pin, uint8_t value);
    uint8_t gets(uint8_t _Pin);
    
private:
    uint8_t _clockPin1;
    uint8_t _serialDataPin1;
    uint8_t _latchPin1;
    
    uint8_t  _digitalValues1[Size1];
};


  /*------------------------------------------- Out Shifting Register -------------------------------------------*/
// ShiftOutRegister constructor
// Size is the number of shiftregisters stacked in serial
template<uint8_t Size>
ShiftOutRegister<Size>::ShiftOutRegister(const uint8_t serialDataPin, const uint8_t clockPin, const uint8_t latchPin)
{
    // set attributes
    _clockPin = clockPin;
    _serialDataPin = serialDataPin;
    _latchPin = latchPin;

    // define pins as outputs
    pinMode(clockPin, OUTPUT);
    pinMode(serialDataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);

    // set pins low
    digitalWrite(clockPin, LOW);
    digitalWrite(serialDataPin, LOW);
    digitalWrite(latchPin, LOW);

    // allocates the specified number of bytes and initializes them to zero
    memset(_digitalValues, 0, Size * sizeof(uint8_t));

    //updateOutRegisters();       // reset shift register
}

// Set all pins of the shift registers at once.
// digitalVAlues is a uint8_t array where the length is equal to the number of shift registers.
template<uint8_t Size>
void ShiftOutRegister<Size>::setAll(const uint8_t * digitalValues)
{
    memcpy( _digitalValues, digitalValues, Size);   // dest, src, size
    updateOutRegisters();
}

/////////////////////////////////////////////////////////////////////
template<uint8_t Size>
void ShiftOutRegister<Size>::scanRows(int value) {
  digitalWrite(_latchPin, LOW); //Pulls the chips latch low
  shiftOut(_serialDataPin, _clockPin, MSBFIRST, value); //Shifts out the 8 bits to the shift register
  digitalWrite(_latchPin, HIGH); //Pulls the latch high displaying the data
}
/////////////////////////////////////////////////////////////////////

// Experimental
// The same as setAll, but the data is located in PROGMEM
// For example with:
//     const uint8_t myFlashData[] PROGMEM = { 0x0F, 0x81 };
#ifdef __AVR__
template<uint8_t Size>
void ShiftOutRegister<Size>::setAll_P(const uint8_t * digitalValuesProgmem)
{
    PGM_VOID_P p = reinterpret_cast<PGM_VOID_P>(digitalValuesProgmem);
    memcpy_P( _digitalValues, p, Size);
    updateOutRegisters();
}
#endif

/*-------------------------------------------------------------------------------*/
// Set a specific pin to either HIGH (1) or LOW (0).
// The pin parameter is a positive, zero-based integer, indicating which pin to set.
template<uint8_t Size>
void ShiftOutRegister<Size>::set(const uint8_t pin, const uint8_t value)
{
    setNoUpdate(pin, value);
    updateOutRegisters();
}

/*-------------------------------------------------------------------------------*/
// Updates the shift register pins to the stored output values.
// This is the function that actually writes data into the shift registers of the 74HC595.
template<uint8_t Size>
void ShiftOutRegister<Size>::updateOutRegisters()
{
    for (int i = Size - 1; i >= 0; i--) {
        shiftOut(_serialDataPin, _clockPin, MSBFIRST, _digitalValues[i]);
    }
    
    digitalWrite(_latchPin, HIGH); 
    digitalWrite(_latchPin, LOW); 
}

/*-------------------------------------------------------------------------------*/
// Equivalent to set(int pin, uint8_t value), except the physical shift register is not updated.
// Should be used in combination with updateOutRegisters().
template<uint8_t Size>
void ShiftOutRegister<Size>::setNoUpdate(const uint8_t pin, const uint8_t value)
{
    (value) ? bitSet(_digitalValues[pin / 8], pin % 8) : bitClear(_digitalValues[pin / 8], pin % 8);
}

/*-------------------------------------------------------------------------------*/
// Sets all pins of all shift registers to HIGH (1).
template<uint8_t Size>
void ShiftOutRegister<Size>::setAllHigh()
{
    for (int i = 0; i < Size; i++) {
        _digitalValues[i] = 255;
    }
    updateOutRegisters();
}

/*-------------------------------------------------------------------------------*/
// Sets all pins of all shift registers to LOW (0).
template<uint8_t Size>
void ShiftOutRegister<Size>::setAllLow()
{
    for (int i = 0; i < Size; i++) {
        _digitalValues[i] = 0;
    }
    updateOutRegisters();
}


/*------------------------------------------- In Shifting Register --------------------------------------------*/
// ShiftInRegister constructor
// Size1 is the number of shiftregisters stacked in serial

#define ShiftInRegister_delay  5
template<uint8_t Size1>
ShiftInRegister<Size1>::ShiftInRegister(const uint8_t serialDataPin1, const uint8_t clockPin1, const uint8_t latchPin1)
{
      _clockPin1 = clockPin1;
      _latchPin1 = latchPin1;
      _serialDataPin1 = serialDataPin1;
      pinMode(clockPin1, OUTPUT);
      pinMode(latchPin1, OUTPUT);
      pinMode(serialDataPin1, INPUT);
      digitalWrite(clockPin1, LOW);
      digitalWrite(latchPin1, HIGH);
      memset(_digitalValues1, 0, Size1);
}

/*-------------------------------------------------------------------------------*/
template<uint8_t Size1>
void ShiftInRegister<Size1>::updateInRegisters()
{
      digitalWrite(_latchPin1, LOW);
#if ShiftInRegister_delay > 0
      delayMicroseconds(ShiftInRegister_delay);
#endif
      digitalWrite(_latchPin1, HIGH);
      for(uint8_t _Pin = 0; _Pin < Size1; _Pin++){
        for(int i = 7; i >= 0; i--){
          uint8_t bit = digitalRead(_serialDataPin1);
          bitWrite(_digitalValues1[_Pin], i, bit);
          digitalWrite(_clockPin1, HIGH);

#if ShiftInRegister_delay > 0
          delayMicroseconds(ShiftInRegister_delay);
#endif
          digitalWrite(_clockPin1, LOW);
        }
      }   
    }
    
/*-------------------------------------------------------------------------------*/
template<uint8_t Size1>
    bool ShiftInRegister<Size1>::get(uint16_t n){
      updateInRegisters();
      return get((n>>3), (n&0x07));
    }
    
/*-------------------------------------------------------------------------------*/
// return true if the pin is active
template<uint8_t Size1>
    bool ShiftInRegister<Size1>::get(uint8_t _Pin, uint8_t value){
      updateInRegisters();
      _Pin = constrain(_Pin, 0, (Size1-1));
      value = constrain(value, 0, 7);
      return !bitRead(_digitalValues1[_Pin], value);
    }
    
/*-------------------------------------------------------------------------------*/
// return all the pins of the specified ShiftInRegister
template<uint8_t Size1>
    uint8_t ShiftInRegister<Size1>::gets(uint8_t _Pin){
      updateInRegisters();
      _Pin = constrain(_Pin, 0, (Size1-1));
      return _digitalValues1[_Pin];
    }
/*-------------------------------------------------------------------------------*/
  
#endif 
