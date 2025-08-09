#ifndef LUA_EMBED_HPP
#define LUA_EMBED_HPP

#include <Arduino.h>
#include <vector>
#include <LittleFS.h>
#include <utility> // Para std::pair
#include "lua.hpp"

using LuaLibInitFunc = int (*)(lua_State *);

class LuaEmbed
{
private:
    lua_State *L = nullptr;
    String scriptPath; // caminho do script se iniciado por ficheiro
    String scriptCode; // código do script se iniciado por string
    bool beginFromScript = false;
    bool beginCalled = false;
    static bool littleFsMounted;

    std::vector<std::pair<String, LuaLibInitFunc>> libsToLoad;

    void checkBeginCalled();

public:
    LuaEmbed() = default;
    ~LuaEmbed();

    void addLib(const String &name, LuaLibInitFunc libInit);

    // Inicia Lua com script de ficheiro
    bool begin(const String &scriptPath);
    // Inicia Lua com script de string
    bool begin_from_script(const String &code);

    bool runScriptFromFile();
    bool runScript(const String &code);

    void loop();
    void reportError();

    // Reinicia o estado Lua e recarrega tudo
    void restart();

    lua_State *getLuaState() { return L; }
    void commandLine();
};

#endif