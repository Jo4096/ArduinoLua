#include "LuaEmbedCJJ.hpp"

static bool littleFsMounted = false; // controla montagem LittleFS

void LuaEmbed::checkBeginCalled()
{
    if (!beginCalled)
    {
        Serial.println("[WARNING]: you forgot to LuaEmbed.begin()/LuaEmbed.begin_from_script()");
        begin_from_script(""); // da begin mas nao faz nenhum codigo extra
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
    if (!littleFsMounted)
    {
        if (!LittleFS.begin())
        {
            Serial.println("Failed to mount LittleFS");
            return false;
        }
        littleFsMounted = true;
    }

    scriptPath = path;

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

    luaL_openlibs(L); // abre libs padrão

    // Carregar as libs personalizadas
    for (auto &libPair : libsToLoad)
    {
        // Chama a função de inicialização. Ela coloca a tabela da biblioteca na pilha.
        libPair.second(L);

        // Atribui a tabela no topo da pilha (o retorno da libInit) à variável global com o nome certo.
        // Isto é o que faltava!
        lua_setglobal(L, libPair.first.c_str());
    }

    return runScriptFromFile();
}

bool LuaEmbed::begin_from_script(const String &code)
{
    beginCalled = true;
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

// O resto do código (runScriptFromFile, runScript, loop, reportError) permanece igual
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

void LuaEmbed::commandLine()
{
    checkBeginCalled();

    static String inputLine;

    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r') // Quando pressiona ENTER
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