#include "MQTT_Agent_Lua.hpp"
#include <map>

#define LUA_MQTT_AGENT_METATABLE "CJJ.MQTT_Agent"

// Guarda referências das funções de comandos Lua
static std::map<String, int> luaCommandHandlers; // command -> Lua ref
static int luaOnMessageCallbackRef = LUA_NOREF; // referência do callback onMessage (se usado)

int lua_MQTT_Agent_gc(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    if (*userdata) {
        delete *userdata;
        *userdata = nullptr;
    }
    return 0;
}

int lua_MQTT_Agent_new(lua_State *L)
{
    const char *ssid = luaL_checkstring(L, 1);
    const char *password = luaL_checkstring(L, 2);
    const char *mqttServer = luaL_checkstring(L, 3);
    const char *mqttUsername = luaL_checkstring(L, 4);
    const char *mqttPassword = luaL_checkstring(L, 5);
    const char *deviceId = luaL_checkstring(L, 6);
    int port = luaL_optinteger(L, 7, 1883);
    int pingPeriod = luaL_optinteger(L, 8, 10000);

    auto **userdata = (MQTT_Agent **)lua_newuserdata(L, sizeof(MQTT_Agent *));
    *userdata = new MQTT_Agent(ssid, password, mqttServer, mqttUsername, mqttPassword, deviceId, port, pingPeriod);

    luaL_getmetatable(L, LUA_MQTT_AGENT_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

int lua_MQTT_Agent_begin(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    bool enablePing = lua_toboolean(L, 2);
    (*userdata)->begin(enablePing);
    return 0;
}

int lua_MQTT_Agent_publish(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    const char *topic = luaL_checkstring(L, 2);
    const char *message = luaL_checkstring(L, 3);
    (*userdata)->publish(topic, message);
    return 0;
}

int lua_MQTT_Agent_loop(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    (*userdata)->loop();
    return 0;
}

int lua_MQTT_Agent_addSubscriptionTopic(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    const char *topic = luaL_checkstring(L, 2);
    (*userdata)->addSubscriptionTopic(String(topic));
    return 0;
}

int lua_MQTT_Agent_publishToDeviceFormatted(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    const char *devId = luaL_checkstring(L, 2);
    const char *command = luaL_checkstring(L, 3);
    const char *message = luaL_checkstring(L, 4);
    (*userdata)->publishToDeviceFormatted(String(devId), String(command), String(message));
    return 0;
}

int lua_MQTT_Agent_registerCommand(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    const char *commandName = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);

    // Guardar função como referência no LUA_REGISTRYINDEX
    lua_pushvalue(L, 3); // copiar a função
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    luaCommandHandlers[String(commandName)] = ref;

    // Registar command handler no C++, captura lua_State* por valor
    (*userdata)->registerCommand(commandName, [L, ref](String from, String topic, JsonDocument &doc) {
        auto it = luaCommandHandlers.find(doc["command"] | "");
        if (it == luaCommandHandlers.end()) return;

        // Recuperar função Lua pelo ref guardado
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

        lua_pushstring(L, from.c_str());
        lua_pushstring(L, topic.c_str());

        String docStr;
        serializeJson(doc, docStr);
        lua_pushstring(L, docStr.c_str());

        if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
            Serial.print("[LUA ERROR] ");
            Serial.println(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    });

    return 0;
}

// --- NOVA FUNÇÃO ---
// Binding para setOnMessageCallback
int lua_MQTT_Agent_setOnMessageCallback(lua_State *L)
{
    auto **userdata = (MQTT_Agent **)luaL_checkudata(L, 1, LUA_MQTT_AGENT_METATABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    // Se já existia uma callback anterior, limpa referência
    if (luaOnMessageCallbackRef != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, luaOnMessageCallbackRef);
        luaOnMessageCallbackRef = LUA_NOREF;
    }

    lua_pushvalue(L, 2);
    luaOnMessageCallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);

    // Registra callback no agente
    (*userdata)->setOnMessageCallback([L](String from, String topic, String payload) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaOnMessageCallbackRef);

        lua_pushstring(L, from.c_str());
        lua_pushstring(L, topic.c_str());
        lua_pushstring(L, payload.c_str());

        if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
            Serial.print("[LUA CALLBACK ERROR] ");
            Serial.println(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    });

    return 0;
}

static const luaL_Reg MQTT_Agent_methods[] = {
    {"begin", lua_MQTT_Agent_begin},
    {"publish", lua_MQTT_Agent_publish},
    {"loop", lua_MQTT_Agent_loop},
    {"addSubscriptionTopic", lua_MQTT_Agent_addSubscriptionTopic},
    {"publishToDeviceFormatted", lua_MQTT_Agent_publishToDeviceFormatted},
    {"registerCommand", lua_MQTT_Agent_registerCommand},
    {"setOnMessageCallback", lua_MQTT_Agent_setOnMessageCallback}, // <<<<< registado aqui
    {NULL, NULL}
};

static const luaL_Reg MQTT_Agent_functions[] = {
    {"new", lua_MQTT_Agent_new},
    {NULL, NULL}
};

extern "C" int luaopen_MQTT_Agent(lua_State *L)
{
    luaL_newmetatable(L, LUA_MQTT_AGENT_METATABLE);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lua_MQTT_Agent_gc);
    lua_setfield(L, -2, "__gc");
    luaL_setfuncs(L, MQTT_Agent_methods, 0);

    lua_newtable(L);
    luaL_setfuncs(L, MQTT_Agent_functions, 0);
    return 1;
}
