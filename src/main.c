#include "luadsl.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

struct da {
    char** items;
    size_t count;
    size_t capacity;
};

int main(void) {

    sp_init();

    struct da new = {0};

    da_grow(&new, sizeof(char*));
    da_append(&new, "hello world\n");
    da_grow(&new, sizeof(char*));
    printf("%s", new.items[0]);
    printf("%zu\n", new.count);

    lua_State* L = luadsl_init();
    luadsl_run(L);

    luadsl_destroy(L);

    

    // lua_State* lua = luaL_newstate();
    //
    // int err = LUA_OK;
    //
    // if ((err = luaL_dofile(lua, "rob.lua")) == LUA_OK) {
    //     lua_pop(lua, -1);
    // } else {
    //     const char* error = lua_tostring(lua, -1);
    //     printf("Lua error %s", error);
    // }
    //
    // lua_close(lua);
    //
    //
    sp_destroy();

    return EXIT_SUCCESS;
}
