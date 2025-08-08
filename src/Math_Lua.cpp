#include "Math_Lua.hpp"
#include <math.h>

//===================================================================================================
// Implementação das Funções
//===================================================================================================

// Funções Trigonométricas
static int l_cos(lua_State *L)
{
    double arg = luaL_checknumber(L, 1);
    double result = cos(arg);
    lua_pushnumber(L, result);
    return 1;
}

static int l_cosd(lua_State *L)
{
    double angle_degrees = luaL_checknumber(L, 1);
    double angle_radians = angle_degrees * (M_PI / 180.0);
    double result = cos(angle_radians);
    lua_pushnumber(L, result);
    return 1;
}

static int l_acos(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = acos(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_acosd(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result_degrees = acos(val) * (180.0 / M_PI);
    lua_pushnumber(L, result_degrees);
    return 1;
}

static int l_sin(lua_State *L)
{
    double arg = luaL_checknumber(L, 1);
    double result = sin(arg);
    lua_pushnumber(L, result);
    return 1;
}

static int l_sind(lua_State *L)
{
    double angle_degrees = luaL_checknumber(L, 1);
    double angle_radians = angle_degrees * (M_PI / 180.0);
    double result = sin(angle_radians);
    lua_pushnumber(L, result);
    return 1;
}

static int l_asin(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = asin(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_asind(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result_degrees = asin(val) * (180.0 / M_PI);
    lua_pushnumber(L, result_degrees);
    return 1;
}

static int l_tan(lua_State *L)
{
    double arg = luaL_checknumber(L, 1);
    double result = tan(arg);
    lua_pushnumber(L, result);
    return 1;
}

static int l_tand(lua_State *L)
{
    double arg_degrees = luaL_checknumber(L, 1);
    double arg_radians = arg_degrees * (M_PI / 180.0);
    double result = tan(arg_radians);
    lua_pushnumber(L, result);
    return 1;
}

static int l_atan(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = atan(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_atand(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result_radians = atan(val);
    double result_degrees = result_radians * (180.0 / M_PI);
    lua_pushnumber(L, result_degrees);
    return 1;
}

static int l_atan2(lua_State *L)
{
    double y = luaL_checknumber(L, 1);
    double x = luaL_checknumber(L, 2);
    double result = atan2(y, x);
    lua_pushnumber(L, result);
    return 1;
}

static int l_atan2d(lua_State *L)
{
    double y = luaL_checknumber(L, 1);
    double x = luaL_checknumber(L, 2);
    double result_radians = atan2(y, x);
    double result_degrees = result_radians * (180.0 / M_PI);
    lua_pushnumber(L, result_degrees);
    return 1;
}

// Funções Matemáticas Gerais
static int l_sqrt(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double res = sqrt(val);
    lua_pushnumber(L, res);
    return 1;
}

static int l_pow(lua_State *L)
{
    double base = luaL_checknumber(L, 1);
    double exponent = luaL_checknumber(L, 2);
    double result = pow(base, exponent);
    lua_pushnumber(L, result);
    return 1;
}

static int l_exp(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = exp(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_log(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = log(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_log10(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = log10(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l__floor(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = floor(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_ceil(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = ceil(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_round(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = floor(val + 0.5);
    lua_pushnumber(L, result);
    return 1;
}

static int l_abs(lua_State *L)
{
    double val = luaL_checknumber(L, 1);
    double result = fabs(val);
    lua_pushnumber(L, result);
    return 1;
}

static int l_min(lua_State *L)
{
    int n_args = lua_gettop(L);
    if (n_args == 0)
        return 0;
    double min_val = luaL_checknumber(L, 1);
    for (int i = 2; i <= n_args; ++i)
    {
        double current_val = luaL_checknumber(L, i);
        if (current_val < min_val)
        {
            min_val = current_val;
        }
    }
    lua_pushnumber(L, min_val);
    return 1;
}

static int l_max(lua_State *L)
{
    int n_args = lua_gettop(L);
    if (n_args == 0)
        return 0;
    double max_val = luaL_checknumber(L, 1);
    for (int i = 2; i <= n_args; ++i)
    {
        double current_val = luaL_checknumber(L, i);
        if (current_val > max_val)
        {
            max_val = current_val;
        }
    }
    lua_pushnumber(L, max_val);
    return 1;
}

//===================================================================================================
// Função de Inicialização da Biblioteca 'math' para o Lua
//===================================================================================================

extern "C" int luaopen_math2(lua_State *L)
{
    static const luaL_Reg math_lib[] = {
        {"cos", l_cos},
        {"cosd", l_cosd},
        {"sin", l_sin},
        {"sind", l_sind},
        {"tan", l_tan},
        {"tand", l_tand},
        {"acos", l_acos},
        {"acosd", l_acosd},
        {"asin", l_asin},
        {"asind", l_asind},
        {"atan", l_atan},
        {"atand", l_atand},
        {"atan2", l_atan2},
        {"atan2d", l_atan2d},
        {"sqrt", l_sqrt},
        {"pow", l_pow},
        {"exp", l_exp},
        {"log", l_log},
        {"log10", l_log10},
        {"ceil", l_ceil},
        {"floor", l__floor},
        {"round", l_round},
        {"abs", l_abs},
        {"min", l_min},
        {"max", l_max},
        {NULL, NULL}};

    luaL_newlib(L, math_lib);

    lua_pushnumber(L, M_PI);
    lua_setfield(L, -2, "pi");

    lua_pushnumber(L, M_E);
    lua_setfield(L, -2, "e");

    return 1;
}