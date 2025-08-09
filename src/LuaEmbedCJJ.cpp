#include "LuaEmbedCJJ.hpp"

bool LuaEmbed::littleFsMounted = false;

void LuaEmbed::checkBeginCalled()
{
    if (!beginCalled)
    {
        Serial.println("[WARNING]: you forgot to call LuaEmbed.begin()/LuaEmbed.begin_from_script()");
        begin_from_script(""); // inicia estado vazio para evitar erros
    }
}

LuaEmbed::~LuaEmbed()
{
    if (L)
    {
        lua_close(L);
        L = nullptr;
    }
}

void LuaEmbed::addLib(const String &name, LuaLibInitFunc libInit)
{
    libsToLoad.push_back({name, libInit});
}

bool LuaEmbed::begin(const String &path)
{
    beginCalled = true;
    beginFromScript = false;
    scriptPath = path;
    scriptCode = "";

    if (!littleFsMounted)
    {
        if (!LittleFS.begin())
        {
            Serial.println("Failed to mount LittleFS");
            return false;
        }
        littleFsMounted = true;
    }

    if (L)
    {
        lua_close(L);
        L = nullptr;
    }

    L = luaL_newstate();
    if (!L)
    {
        Serial.println("Failed to create Lua state");
        return false;
    }

    luaL_openlibs(L);

    for (auto &libPair : libsToLoad)
    {
        libPair.second(L);
        lua_setglobal(L, libPair.first.c_str());
    }

    return runScriptFromFile();
}

bool LuaEmbed::begin_from_script(const String &code)
{
    beginCalled = true;
    beginFromScript = true;
    scriptCode = code;
    scriptPath = "";

    if (!littleFsMounted)
    {
        if (!LittleFS.begin())
        {
            Serial.println("Failed to mount LittleFS");
            return false;
        }
        littleFsMounted = true;
    }

    if (L)
    {
        lua_close(L);
        L = nullptr;
    }

    L = luaL_newstate();
    if (!L)
    {
        Serial.println("Failed to create Lua state");
        return false;
    }

    luaL_openlibs(L);

    for (auto &libPair : libsToLoad)
    {
        libPair.second(L);
        lua_setglobal(L, libPair.first.c_str());
    }

    return runScript(code);
}

bool LuaEmbed::runScriptFromFile()
{
    checkBeginCalled();

    if (!L)
    {
        Serial.println("Lua state not initialized");
        return false;
    }
    if (!LittleFS.exists(scriptPath))
    {
        Serial.printf("Script file not found: %s\n", scriptPath.c_str());
        return false;
    }
    File f = LittleFS.open(scriptPath, "r");
    if (!f)
    {
        Serial.printf("Failed to open script file: %s\n", scriptPath.c_str());
        return false;
    }
    String code;
    while (f.available())
    {
        code += (char)f.read();
    }
    f.close();
    return runScript(code);
}

bool LuaEmbed::runScript(const String &code)
{
    checkBeginCalled();

    if (!L)
    {
        Serial.println("Lua state not initialized");
        return false;
    }
    if (luaL_dostring(L, code.c_str()) != LUA_OK)
    {
        reportError();
        return false;
    }
    return true;
}

void LuaEmbed::loop()
{
    checkBeginCalled();

    if (!L)
        return;

    lua_getglobal(L, "loop");
    if (lua_isfunction(L, -1))
    {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK)
        {
            reportError();
        }
    }
    else
    {
        lua_pop(L, 1);
    }
}

void LuaEmbed::reportError()
{
    checkBeginCalled();

    if (!L)
        return;

    const char *msg = lua_tostring(L, -1);
    Serial.printf("Lua Error: %s\n", msg);
    lua_pop(L, 1);
}

void LuaEmbed::restart()
{
    if (L)
    {
        lua_close(L);
        L = nullptr;
    }

    if (!beginCalled)
    {
        Serial.println("[WARNING]: restart called but LuaEmbed.begin() was never called");
        return;
    }

    L = luaL_newstate();
    if (!L)
    {
        Serial.println("Failed to recreate Lua state during restart");
        return;
    }

    luaL_openlibs(L);

    for (auto &libPair : libsToLoad)
    {
        libPair.second(L);
        lua_setglobal(L, libPair.first.c_str());
    }

    if (beginFromScript)
    {
        if (!runScript(scriptCode))
        {
            Serial.println("Failed to rerun script from string during restart");
        }
    }
    else
    {
        if (!runScriptFromFile())
        {
            Serial.println("Failed to rerun script from file during restart");
        }
    }
}

void LuaEmbed::commandLine()
{
    checkBeginCalled();

    static String inputLine;

    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r') // Enter
        {
            if (inputLine.length() > 0)
            {
                Serial.print(">> ");
                Serial.println(inputLine);

                if (luaL_dostring(L, inputLine.c_str()) != LUA_OK)
                {
                    reportError();
                }

                inputLine = "";
            }
        }
        else
        {
            inputLine += c;
        }
    }
}