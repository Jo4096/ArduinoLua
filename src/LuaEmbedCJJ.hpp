#ifndef LUA_EMBED_HPP
#define LUA_EMBED_HPP

#include <Arduino.h>
#include <vector>
#include <LittleFS.h>
#include <utility> // std::pair
#include "lua.hpp"

using LuaLibInitFunc = int (*)(lua_State *);

class LuaEmbed
{
private:
    lua_State *L = nullptr;
    String scriptPath;
    String scriptCode;
    bool beginFromScript = false;
    bool beginCalled = false;
    long lastGcMillis = 0;
    long gcInterval = 10000;
    std::vector<std::pair<String, LuaLibInitFunc>> libsToLoad;

    void checkBeginCalled();

public:
    LuaEmbed() = default;
    ~LuaEmbed();

    void addLib(const String &name, LuaLibInitFunc libInit);

    bool begin(const String &scriptPath, long gcInterval = 10000);
    bool begin_from_script(const String &code, long gcInterval = 10000);

    bool runScriptFromFile();
    bool runScript(const String &code, bool cleancg = false);

    void loop();
    void reportError();

    void restart();

    lua_State *getLuaState() { return L; }
    void commandLine();

    void forceGC();
};

#endif
