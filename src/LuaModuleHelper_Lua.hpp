#ifndef LUAMODULEHELPER_LUA
#define LUAMODULEHELPER_LUA

#include "lua.hpp"
#include <new>

namespace LuaHelper
{
    template <typename T, typename... Args>
    T *newObjectWithMeta(lua_State *L, const char *metaname, Args &&...args)
    {
        void *mem = lua_newuserdata(L, sizeof(T));
        T *obj = new (mem) T(std::forward<Args>(args)...);

        luaL_getmetatable(L, metaname);
        lua_setmetatable(L, -2);

        return obj;
    }

    template <typename T>
    int gc(lua_State *L)
    {
        T *obj = static_cast<T *>(lua_touserdata(L, 1));
        if (obj)
            obj->~T();
        return 0;
    }

    inline void registerMetatable(lua_State *L, const char *name, const luaL_Reg *methods, lua_CFunction gc_func)
    {
        luaL_newmetatable(L, name);
        lua_newtable(L);
        luaL_setfuncs(L, methods, 0);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, gc_func);
        lua_setfield(L, -2, "__gc");

        lua_pop(L, 1);
    }
}

#endif