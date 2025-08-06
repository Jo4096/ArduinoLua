#ifndef LUA_EMBED_HPP
#define LUA_EMBED_HPP

#include <Arduino.h>
#include <vector>
#include <LittleFS.h>
#include <utility> // Para std::pair

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using LuaLibInitFunc = int (*)(lua_State *);

class LuaEmbed
{
private:
    lua_State *L = nullptr;
    String scriptPath;
    // Armazena pares de nome da biblioteca e a função de inicialização
    std::vector<std::pair<String, LuaLibInitFunc>> libsToLoad;

public:
    LuaEmbed() = default;
    ~LuaEmbed();

    // Adicionar biblioteca para carregar com um nome
    void addLib(const String &name, LuaLibInitFunc libInit);

    // Inicializar Lua e carregar as libs adicionadas a partir de um ficheiro
    bool begin(const String &scriptPath);
    // Inicializar Lua e carregar as libs adicionadas a partir de uma string
    bool begin_from_script(const String &code);

    bool runScriptFromFile();
    bool runScript(const String &code);

    void loop();
    void reportError();

    lua_State *getLuaState() { return L; }
};

#endif