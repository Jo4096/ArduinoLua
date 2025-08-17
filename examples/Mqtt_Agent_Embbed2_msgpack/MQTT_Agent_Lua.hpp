#ifndef MQTT_AGENT_LUA_HPP
#define MQTT_AGENT_LUA_HPP

#include "Arduino.h"
#include "MQTT_Agent.hpp"
#include "ArduinoLua.hpp"
#include "lua.hpp"

void jsonToLuaTable(lua_State* L, JsonDocument& doc);

static int l_mqtt_config(lua_State *L);
static int l_mqtt_begin(lua_State *L);
static int l_mqtt_loop(lua_State *L);
static int l_mqtt_stop(lua_State *L);
static int l_mqtt_addSubscriptionTopic(lua_State *L);
static int l_mqtt_publish(lua_State *L);
static int l_mqtt_publishToDevice(lua_State *L);
static int l_mqtt_publishToDeviceFormatted(lua_State *L);
static int l_mqtt_setOnMessageCallback(lua_State *L);
static int l_mqtt_registerCommand(lua_State *L);
static int l_mqtt_removeCommand(lua_State *L);
static int l_mqtt_getDeviceId(lua_State *L);
static int l_mqtt_getCommands(lua_State *L);
static int l_mqtt_getKnownDevices(lua_State *L);

static int l_mqtt_fileloop(lua_State *L);
static int l_mqtt_transfer_file(lua_State *L);


extern "C" int luaopen_MQTT_Agent(lua_State *L);

#endif
