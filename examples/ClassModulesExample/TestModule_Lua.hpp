#ifndef MODULO_TEST_HPP
#define MODULO_TEST_HPP

#include <ArduinoLua.hpp>
// Inclui o helper para módulos Lua, que simplifica a criação de classes.
#include "LuaModuleHelper_Lua.hpp"

#include "SimpleClass.hpp"

// Declarações das funções C++ que serão expostas ao Lua.
// Usamos a convenção 'l_' para indicar que são funções Lua (lua functions).
// O sufixo indica a sua função, como 'new', 'set', 'print', etc.
static int l_printar_new(lua_State *L);
static int l_printar_set(lua_State *L);
static int l_printar_print(lua_State *L);
static int l_printar_print_ao_contrario(lua_State *L);
static int l_printar_gc(lua_State *L);

// A função de entrada principal do módulo.
// 'extern "C"' é necessário para garantir que o C++ não altere o nome da função (name mangling),
// permitindo que o Lua a encontre.
extern "C" int luaopen_SimpleClass(lua_State *L);

#endif