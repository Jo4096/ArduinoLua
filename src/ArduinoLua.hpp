#ifndef ARDUINOLUA_HPP
#define ARDUINOLUA_HPP



extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


#include "Arduino.h" // Inclui o cabeçalho do Arduino para as funções de GPIO
#include "Wire.h"
#include "SPI.h"

#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// STD FUNCTIONS

///////////////////////
// ARDUINO FUNCTIONS //
///////////////////////

// Função para digitalWrite()
// Sintaxe Lua: arduino.digitalWrite(pin, value)
static int l_digitalWrite(lua_State *L);

// Função para digitalRead()
// Sintaxe Lua: value = arduino.digitalRead(pin)
static int l_digitalRead(lua_State *L);

static int l_analogWrite(lua_State *L);

static int l_analogRead(lua_State *L);

static int l_pinMode(lua_State *L);

static int l_serial_print(lua_State *L);

static int l_serial_println(lua_State *L);

static int l_delay(lua_State *L);

static int l_millis(lua_State *L);

static int l_micros(lua_State *L);

static int l_delayMicroseconds(lua_State *L);

static int l_map(lua_State *L);

static int l_constrain(lua_State *L);

static int l_pulseIn(lua_State *L);

static int l_serial_begin(lua_State *L);

static int l_serial_available(lua_State *L);

static int l_serial_read(lua_State *L);

static int l_tone(lua_State *L);

static int l_noTone(lua_State *L);

////////////////////////
//      Wire.h        //
////////////////////////

static int l_i2cBegin(lua_State *L);

static int l_i2cBeginTransmission(lua_State *L);

static int l_i2cWrite(lua_State *L);

static int l_i2cEndTransmission(lua_State *L);

static int l_i2cRequestFrom(lua_State *L);

static int l_i2cAvailable(lua_State *L);

static int l_i2cRead(lua_State *L);

///////////////////////////////
//          SPI.h            //
///////////////////////////////

static int l_spiBegin(lua_State *L);

static int l_spiTransfer(lua_State *L);

extern "C" int luaopen_arduino(lua_State *L);

/////////////////////////////////////////////////////////////////////////////////////////////////////

#endif