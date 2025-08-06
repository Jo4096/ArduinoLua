#ifndef MQTT_AGENT_LUA_HPP
#define MQTT_AGENT_LUA_HPP

#include "Arduino.h"
#include <memory>
#include "MQTT_Agent.hpp"
#include "ArduinoLua.hpp"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#define LUA_MQTT_AGENT_METATABLE "CJJ.MQTT_Agent"

// Funções do binding Lua
int lua_MQTT_Agent_gc(lua_State *L);
int lua_MQTT_Agent_new(lua_State *L);
int lua_MQTT_Agent_begin(lua_State *L);
int lua_MQTT_Agent_publish(lua_State *L);
int lua_MQTT_Agent_loop(lua_State *L);
int lua_MQTT_Agent_addSubscriptionTopic(lua_State *L);
int lua_MQTT_Agent_publishToDeviceFormatted(lua_State *L);
int lua_MQTT_Agent_registerCommand(lua_State *L);
int lua_MQTT_Agent_setOnMessageCallback(lua_State *L);

extern "C" int luaopen_MQTT_Agent(lua_State *L);

#endif
