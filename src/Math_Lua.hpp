#ifndef MATH_LUA_HPP
#define MATH_LUA_HPP

#include "lua.hpp"
#include "Arduino.h" // Inclui o cabeçalho do Arduino para as funções de GPIO
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// STD FUNCTIONS

static int l_cos(lua_State *L);
static int l_cosd(lua_State *L);
static int l_acos(lua_State *L);
static int l_acosd(lua_State *L);

static int l_sin(lua_State *L);
static int l_sind(lua_State *L);
static int l_asin(lua_State *L);
static int l_asind(lua_State *L);

static int l_tan(lua_State *L);
static int l_tand(lua_State *L);
static int l_atan(lua_State *L);
static int l_atand(lua_State *L);
static int l_atan2(lua_State *L);
static int l_atan2d(lua_State *L);

static int l_sqrt(lua_State *L);
static int l_pow(lua_State *L);
static int l_exp(lua_State *L);
static int l_log(lua_State *L);
static int l_log10(lua_State *L);

static int l__floor(lua_State *L);
static int l_ceil(lua_State *L);
static int l_round(lua_State *L);
static int l_abs(lua_State *L);
static int l_min(lua_State *L);
static int l_max(lua_State *L);

extern "C" int luaopen_math2(lua_State *L);

/////////////////////////////////////////////////////////////////////////////////////////////////////

#endif