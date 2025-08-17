#include "MQTT_Agent_Lua.hpp"

struct LuaCallbackInfo {
    lua_State* L = nullptr;
    int ref = LUA_NOREF;
};

static LuaCallbackInfo onMessageCallbackInfo;
static std::map<std::string, LuaCallbackInfo> commandCallbacks;

void jsonToLuaTable(lua_State* L, JsonDocument& doc) 
{
    lua_newtable(L);
    int tableIndex = lua_gettop(L);

    if (doc.is<JsonObject>()) {
        for (JsonPair p : doc.as<JsonObject>()) {
            const char* key = p.key().c_str();
            JsonVariant value = p.value();

            lua_pushstring(L, key);

            if (value.is<int>()) {
                lua_pushinteger(L, value.as<int>());
            } else if (value.is<float>()) {
                lua_pushnumber(L, value.as<float>());
            } else if (value.is<bool>()) {
                lua_pushboolean(L, value.as<bool>());
            } else if (value.is<const char*>()) {
                lua_pushstring(L, value.as<const char*>());
            } else if (value.is<JsonObject>()) {
                JsonDocument nestedDoc;
                deserializeJson(nestedDoc, value.as<String>());
                jsonToLuaTable(L, nestedDoc);
            } else if (value.is<JsonArray>()) {
                JsonDocument nestedDoc;
                deserializeJson(nestedDoc, value.as<String>());
                jsonToLuaTable(L, nestedDoc);
            } else {
                lua_pushnil(L);
            }
            lua_settable(L, tableIndex);
        }
    } else if (doc.is<JsonArray>()) {
        int i = 1;
        for (JsonVariant element : doc.as<JsonArray>()) {
            lua_pushinteger(L, i++);

            if (element.is<int>()) {
                lua_pushinteger(L, element.as<int>());
            } else if (element.is<float>()) {
                lua_pushnumber(L, element.as<float>());
            } else if (element.is<bool>()) {
                lua_pushboolean(L, element.as<bool>());
            } else if (element.is<const char*>()) {
                lua_pushstring(L, element.as<const char*>());
            } else if (element.is<JsonObject>()) {
                JsonDocument nestedDoc;
                deserializeJson(nestedDoc, element.as<String>());
                jsonToLuaTable(L, nestedDoc);
            } else if (element.is<JsonArray>()) {
                JsonDocument nestedDoc;
                deserializeJson(nestedDoc, element.as<String>());
                jsonToLuaTable(L, nestedDoc);
            } else {
                lua_pushnil(L);
            }
            lua_settable(L, tableIndex);
        }
    }
}


static int l_mqtt_config(lua_State *L) {
    const char *ssid = luaL_checkstring(L, 1);
    const char *password = luaL_checkstring(L, 2);
    const char *mqttServer = luaL_checkstring(L, 3);
    const char *mqttUsername = luaL_checkstring(L, 4);
    const char *mqttPassword = luaL_checkstring(L, 5);
    const char *deviceId = luaL_checkstring(L, 6);
    int port = luaL_optinteger(L, 7, 1883);
    int pingPeriod = luaL_optinteger(L, 8, 30000);
    int size = luaL_optinteger(L, 9, 1024);

    Agent.config(ssid, password, mqttServer, mqttUsername, mqttPassword, deviceId, port, pingPeriod);
    Agent.setBufferSize(size);
    return 0;
}

static int l_mqtt_begin(lua_State *L) {
    bool enable = luaL_opt(L, lua_toboolean, 1, true);
    bool enable2 = luaL_opt(L, lua_toboolean, 2, true);
    bool correct = Agent.begin(enable, enable2);
    lua_pushboolean(L, correct);
    return 1;
}

static int l_mqtt_loop(lua_State *L) {
    bool enable = luaL_opt(L, lua_toboolean, 1, true);
    Agent.loop();
    if (enable) {
        const char* filename = luaL_optstring(L, 2, "");
        bool overwrite = lua_isnoneornil(L, 3) ? true : lua_toboolean(L, 3);
        String result = Agent.fileLoop(LittleFS, filename, overwrite);
        lua_pushstring(L, result.c_str());
        return 1;
    }
    return 0;
}

static int l_mqtt_stop(lua_State *L) {
    Agent.stop();
    return 0;
}

static int l_mqtt_addSubscriptionTopic(lua_State *L) {
    String topic = luaL_checkstring(L, 1);
    Agent.addSubscriptionTopic(topic);
    return 0;
}

static int l_mqtt_publish(lua_State *L) {
    String topic = luaL_checkstring(L, 1);
    String message = luaL_checkstring(L, 2);
    Agent.publish(topic, message);
    return 0;
}

static int l_mqtt_publishToDevice(lua_State *L) {
    String devId = luaL_checkstring(L, 1);
    String message = luaL_checkstring(L, 2);
    Agent.publishToDevice(devId, message);
    return 0;
}

static int l_mqtt_publishToDeviceFormatted(lua_State *L) {
    String devId = luaL_checkstring(L, 1);
    String command = luaL_checkstring(L, 2);
    String message = luaL_checkstring(L, 3);
    Agent.publishToDeviceFormatted(devId, command, message);
    return 0;
}

static int l_mqtt_setOnMessageCallback(lua_State *L) 
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    
    if (onMessageCallbackInfo.ref != LUA_NOREF) {
        luaL_unref(onMessageCallbackInfo.L, LUA_REGISTRYINDEX, onMessageCallbackInfo.ref);
    }
    
    onMessageCallbackInfo.L = L;
    onMessageCallbackInfo.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    int lua_func_ref = onMessageCallbackInfo.ref; // **Aqui!**
    lua_State* callback_L = L;

    // Use a variável lua_func_ref na captura
    std::function<void(String, String, String)> cpp_callback = [callback_L, lua_func_ref](String topic, String payload, String from) {
        lua_rawgeti(callback_L, LUA_REGISTRYINDEX, lua_func_ref);
        lua_pushstring(callback_L, topic.c_str());
        lua_pushstring(callback_L, payload.c_str());
        lua_pushstring(callback_L, from.c_str());
        if (lua_pcall(callback_L, 3, 0, 0) != LUA_OK) {
            const char *error_msg = lua_tostring(callback_L, -1);
            lua_pop(callback_L, 1);
        }
    };
    Agent.setOnMessageCallback(cpp_callback);
    return 0;
}

static int l_mqtt_registerCommand(lua_State *L) {
    const char *name_c_str = luaL_checkstring(L, 1);
    std::string name(name_c_str);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    if (commandCallbacks.count(name)) {
        luaL_unref(commandCallbacks[name].L, LUA_REGISTRYINDEX, commandCallbacks[name].ref);
    }

    int lua_func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    commandCallbacks[name] = {L, lua_func_ref};

    lua_State* callback_L = L;
    std::function<void(String, String, JsonDocument &)> cpp_callback =
        [callback_L, lua_func_ref](String from, String topic, JsonDocument &doc) {
        lua_rawgeti(callback_L, LUA_REGISTRYINDEX, lua_func_ref);
        lua_pushstring(callback_L, from.c_str());
        lua_pushstring(callback_L, topic.c_str());
        jsonToLuaTable(callback_L, doc);
        if (lua_pcall(callback_L, 3, 0, 0) != LUA_OK) {
            const char *error_msg = lua_tostring(callback_L, -1);
            lua_pop(callback_L, 1);
        }
    };
    Agent.registerCommand(String(name_c_str), cpp_callback);
    return 0;
}

static int l_mqtt_removeCommand(lua_State *L) {
    const char *name_c_str = luaL_checkstring(L, 1);
    std::string name(name_c_str);

    if (commandCallbacks.count(name)) {
        luaL_unref(commandCallbacks[name].L, LUA_REGISTRYINDEX, commandCallbacks[name].ref);
        commandCallbacks.erase(name);
    }
    Agent.removeCommand(String(name_c_str));
    return 0;
}


static int l_mqtt_getDeviceId(lua_State *L)
{
    lua_pushstring(L, Agent.getDeviceId().c_str());
    return 1;
}

static int l_mqtt_getCommands(lua_State *L)
{
    lua_newtable(L);
    int i = 1;
    for (const String& dev : Agent.getCommands()) {
        lua_pushinteger(L, i++);
        lua_pushstring(L, dev.c_str());
        lua_settable(L, -3);
    }
    return 1;
}


static int l_mqtt_getKnownDevices(lua_State *L)
{
    lua_newtable(L);
    int i = 1;
    for (const String& dev : Agent.getKnownDevices()) {
        lua_pushinteger(L, i++);
        lua_pushstring(L, dev.c_str());
        lua_settable(L, -3);
    }
    return 1;
}

static int l_mqtt_transfer_file(lua_State *L)
{
    String destination_id = String(luaL_checkstring(L, 1));
    String filename = String(luaL_checkstring(L, 2));

    luaL_checktype(L, 3, LUA_TTABLE);  // garante que o 3º arg é tabela
    size_t len = lua_rawlen(L, 3);

    std::vector<uint8_t> rawdata;
    rawdata.reserve(len);

    for (size_t i = 1; i <= len; i++)  // Lua é 1-based
    {
        lua_rawgeti(L, 3, i);                  // empilha tabela[ i ]
        int value = luaL_checkinteger(L, -1);  // lê inteiro
        lua_pop(L, 1);                         // remove da stack
        rawdata.push_back((uint8_t)value);
    }

    int chunkSize = luaL_optinteger(L, 4, 512);

    Agent.transfer_file(destination_id, filename, rawdata, chunkSize);

    return 0;
}


static const luaL_Reg mqtt_lib[] = {
    {"config", l_mqtt_config},
    {"begin", l_mqtt_begin},
    {"loop", l_mqtt_loop},
    {"stop", l_mqtt_stop},
    {"addSubscriptionTopic", l_mqtt_addSubscriptionTopic},
    {"publish", l_mqtt_publish},
    {"publishToDevice", l_mqtt_publishToDevice},
    {"publishToDeviceFormatted", l_mqtt_publishToDeviceFormatted},
    {"setOnMessageCallback", l_mqtt_setOnMessageCallback},
    {"registerCommand", l_mqtt_registerCommand},
    {"removeCommand", l_mqtt_removeCommand},
    {"getDeviceId", l_mqtt_getDeviceId},
    {"getCommands", l_mqtt_getCommands},
    {"getKnownDevices", l_mqtt_getKnownDevices},
    {"transfer_file",l_mqtt_transfer_file},
    {NULL, NULL}
};

// Função de inicialização da biblioteca Lua
extern "C" int luaopen_MQTT_Agent(lua_State *L) 
{
    luaL_newlib(L, mqtt_lib);
    return 1;
}