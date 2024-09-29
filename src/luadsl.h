#ifndef ROB_LUADSL_H_
#define ROB_LUADSL_H_

#include "lua.h"


typedef enum {
    // ld
    link,

    // C
    cc,

    // Utils
    make,
    command,
} target_type;

typedef struct {
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } input_files;
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } output_files;
    struct {
        struct {
            char** items;
            size_t count;
            size_t capacity;
        } link_libraries;
        struct {
            char** items;
            size_t count;
            size_t capacity;
        } library_dirs;
    } options;
} link_target;

typedef struct {
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } input_files;
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } output_files;

    struct {
        struct {
            char** items;
            size_t count;
            size_t capacity;
        } include_dirs;
    } options;
} cc_target;

typedef struct {
    char* directory;
    struct {
        char* items;
        size_t count;
        size_t capacity;
    } make_targets;
} make_target;

typedef struct {
    char* command;
    struct {
        char* items;
        size_t count;
        size_t capacity;
    } args;
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } input_files;
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } output_files;
} command_target;

typedef union {
    // link target
    link_target link;
    // cc target
    cc_target cc;
    // make target
    make_target make;
    // command target
    command_target command;
} target_data;


typedef struct target {
    target_type type;
    struct {
        struct target *items;
        size_t count;
        size_t capacity;
    } deps;
    target_data data;
} target;

/// Initalizes a lua state with the complete dsl.
lua_State* luadsl_init(void);
void luadsl_run(lua_State* L);
void luadsl_destroy(lua_State* L);

#endif //  ROB_LUADSL_H_
