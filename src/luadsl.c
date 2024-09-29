#include "luadsl.h"
#include "lua.h"
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

static const luaL_Reg dslLibs[] = {
    {LUA_GNAME, luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    // {LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_UTF8LIBNAME, luaopen_utf8},
    {NULL, NULL},
};

#define CCOPTS_INCDIRS "include_dirs"

static void check_ccOptions(lua_State* L, int arg) {
    luaL_argcheck(L, lua_istable(L, arg), arg, "ccOptions has to be a table");
    int type = lua_getfield(L, arg, CCOPTS_INCDIRS);
    luaL_argcheck(L, type == LUA_TTABLE, arg, "ccOptions should have " CCOPTS_INCDIRS " field that is a table");
}

// TODO: Check if the table is an actual `ccOptions`.
static int luadsl_c_ccOptions_includeDir(lua_State* L) {
    check_ccOptions(L, 1);
    const char* str = luaL_checkstring(L, 2);
    lua_getfield(L, 1, CCOPTS_INCDIRS);
    lua_Integer len = luaL_len(L, -1);
    len = luaL_intop(+, len, 1);
    lua_pushstring(L, str);
    lua_seti(L, -2, len);

    return 0;
}

static int luadsl_c_buildObjs(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    check_ccOptions(L, 2);

    lua_newtable(L);

    return 1;
}

static int luadsl_c_ccOptions(lua_State* L) {
    lua_checkstack(L, 5);
    lua_createtable(L, 0, 1); // 1 = {}
    lua_newtable(L); // 2 = {}
    lua_setfield(L, -2, CCOPTS_INCDIRS); // 1[CCOP...] = 2
    lua_pushcfunction(L, luadsl_c_ccOptions_includeDir); // func = 3
    lua_setfield(L, -2, "includeDir"); // 1[includeDir] = 
    return 1;
}



lua_State* luadsl_init(void) {
    lua_State* L = luaL_newstate();

    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = dslLibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }

    // c
    luaL_Reg cLibs[] = {{"ccOptions", luadsl_c_ccOptions}, {"buildObjs", luadsl_c_buildObjs}};

    luaL_newlibtable(L, cLibs); // -1 - t - c
    for (size_t i = 0; i < (sizeof(cLibs) / sizeof(cLibs[0])); i++) {
        lua_pushcfunction(L, cLibs[i].func);
        lua_setfield(L, -2, cLibs[i].name);
    }

    lua_createtable(L, 0, 1);   // 2 - t - rob
                                // 1 - t - c
    //
    lua_insert(L, -2);          // 2 - t - c
                                // 1 - t - rob

    lua_setfield(L, -2, "c"); // -2(rob)["c"] = c


    lua_setglobal(L, "rob");

    return L;
}

void luadsl_run(lua_State* L) {
    if(luaL_dofile(L, "rob.lua") == LUA_OK) {
        lua_pop(L, -1);
    } else {
        const char* error = lua_tostring(L, -1);
        fprintf(stderr, "[luadsl] Lua error: %s", error);
    }
}

void luadsl_destroy(lua_State* L) {
    lua_close(L);
}
