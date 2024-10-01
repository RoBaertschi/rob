/**
 * ROB Build System.
 *
 *  MIT License
 *
 *  Copyright (c) 2024 Robin Bärtschi
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */
#ifndef ROB_H_
#define ROB_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#if !defined (ROB_IMPLEMENTATION) && defined (ROB_IMPL)
#   define ROB_IMPLEMENTATION
#endif

#ifndef ROB_BIN_FILE
#   define ROB_BIN_FILE "./rob"
#endif

#ifndef ROB_C_FILE
#   define ROB_C_FILE __FILE__
//  There is no c file specified, so we find it ourself.
#   define ROB_NO_C_FILE 1
#else
#   define ROB_NO_C_FILE 0
#endif

#if defined (__unix__) || defined (__APPLE__) && defined (__MACH__)
#   define ROB_POSIX
#   define _POSIX_C_SOURCE 200809L
#else
#error "unsupported platform, this might be added later. Consider contributing if you need this platform."
#endif

// Utilities

// ================
// ------ da ------
// da is a dynamic array, it should contain the following members:
// *items   - A pointer to a item
// count    - A integer type for the size, often a size_t
// capacity - A integer type for the capacity, often a size_t
// 
// The idea is from tscoding, the implementation is by me yanked from the lua branch.
// ================

#define ROB_DA_INITIAL_CAP 4

/// Returns the new capacity
#define rob_da_grow(da, item_size) (\
        (da)->items != NULL \
            ? ( (da)->items = realloc( (da)->items, item_size * (da)->capacity * 2 ) )\
                == NULL ? \
                    ((da)->capacity = 0, (da)->count = 0) \
                    : ((da)->capacity *= 2)\
            : ( ( (da)->items = malloc(ROB_DA_INITIAL_CAP * item_size) ) == NULL\
                ? 0\
                : ((da)->capacity = ROB_DA_INITIAL_CAP)\
            )\
        )

#define rob_da_ensure_size(da, size, item_size) \
    while((da)->capacity < size) {\
        rob_da_grow((da), item_size);\
    }


#define rob_da_append(da, item) {rob_da_ensure_size(( da ), ( da )->count + 1, sizeof( item ) );\
                            ((da)->items[((da)->count)++] = item);\
}


#define rob_da_pop(da) {((da)->items[((da)->count = ((da)->count - 1))]);}

#define rob_da_str_append(da, ...) { char* items[] = { __VA_ARGS__ }; size_t item_size = sizeof(items) / sizeof(char*); for(size_t i = 0; i < item_size; i++) rob_da_append((da), items[i]); }

// Build system

// ================
// Rob Build System
//  The build system is currently a really basic system to
//  build a vararity of software. Currently rob can automatically rebuild
//  itself and also run other commands. It is designed to be cross plastform
//  but currently only support posix systems.
//
// Usage:
//  To get started, include rob.h and add the ROB_IMPL define. 
//  Then add `rob_rebuild_self();` to the start of the main function.
//  This function will check if rob.h or the source file are newer then the executable "rob"
//  and rebuild it when it is out-of-date.
//  
//  Now you can use rob_create_command_builder(char* command) to create a
//  command builder. The command builder is a dynamic array(see above about da) of args and 
//  a command. You then can use all rob_da_* functions on it and also
//  rob_command_append(builder, ...) that appends multiple strings.
//  You then can call rob_run_command(rob_command_builder* builder) to run the command.
//  This will fork and execve the command and print its outputs to stdout.
// ================


typedef struct {
    char** items;
    size_t count;
    size_t capacity;
    char* command_name;
} rob_command_builder;


#define rob_command_append(command, ...) rob_da_str_append(command, __VA_ARGS__)

#define rob_rebuild_self() (rob__rebuild_self(ROB_C_FILE, ROB_BIN_FILE))

rob_command_builder rob_create_command_builder(char* command_name);
void rob_run_command(rob_command_builder* b);


typedef struct rob_target {
    rob_command_builder b;
    struct {
        char** items;
        size_t count;
        size_t capacity;
    } inputs;
    // instead of a file, you can specify a target
    struct {
        struct rob_target** items;
        size_t count;
        size_t capacity;
    } input_target_dependencies;
    const char* output;
} rob_target;

typedef struct {
    bool is_outdated;
    rob_target** items;
    size_t count;
    size_t capacity;
} rob_outdated_targets;

rob_outdated_targets rob_target_outdated(rob_target* t);
// Do not use the builder anymore after calling this function.
rob_target rob_target_create(rob_command_builder b, const char* output);
void rob_sync_target(rob_target* target);
#define rob_target_inputs_append(t, ...) rob_da_str_append(&(t)->inputs, __VA_ARGS__)
#define rob_target_inputs_append_target(t, item) rob_da_append(&(t)->input_target_dependencies, item)

// Checks wether file a is newer then file b.
// Returns -1 on error, else true or false
int rob_is_newer_than(const char* a, const char* b);

typedef struct {
    struct {
        rob_target* items;
        size_t count;
        size_t capacity;
    } targets;
} rob_state;

void rob__rebuild_self(char* c_file, char* bin_file);
bool rob__is_outdated(char* c_file, char* bin_file);
bool rob__is_newer_than_or_error(char* c_file, char* bin_file);
void rob__compile_self(char* c_file, char* bin_file);

#ifdef ROB_IMPLEMENTATION

#define INFO_FMT(fmt, ...) fprintf(stdout, __FILE__ ":%d"  " [rob] INFO - " fmt, __LINE__, __VA_ARGS__)
#define INFO(fmt) fprintf(stderr, __FILE__ ":%d"  " [rob] INFO - " fmt "\n", __LINE__)
#define FATAL_FMT(fmt, ...) fprintf(stderr, __FILE__ ":%d" " [rob] FATAL - " fmt, __LINE__,  __VA_ARGS__)
#define FATAL(fmt) fprintf(stderr, __FILE__ ":%d" " [rob] FATAL - " fmt "\n", __LINE__)

#ifdef ROB_POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <spawn.h>
#endif


rob_command_builder rob_create_command_builder(char* command_name) {
    rob_command_builder b = (rob_command_builder){
        .items = NULL,
        .count = 0,
        .capacity = 0,
        .command_name = command_name,
    };

    rob_da_append(&b, command_name);

    return b;
}

void rob_run_command(rob_command_builder* b) {
#ifdef ROB_POSIX
    INFO_FMT("spawning command \"%s", b->command_name);
    for (size_t i = 1; i < b->count; i++) {
        fprintf(stdout, " %s", b->items[i]);
    }
    fprintf(stdout, "\"\n");

    int pipefd[2] = {0, 0};
    errno = 0; // reset errno for correct error
    int err = pipe(pipefd);
    if (err != 0) {
        FATAL_FMT("could not create pipe, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int p_read, p_write = 0;
    p_read = pipefd[0];
    p_write = pipefd[1];

    errno = 0; // reset errno for correct error
    pid_t pid = fork();
    // Error
    if (pid < 0) {
        FATAL_FMT("could not spawn process, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // child process
        dup2(p_write, STDOUT_FILENO);
        close(p_read);
        close(p_write);

        if (execvp(b->command_name, b->items) != 0) {
            perror("execvp");
        }
        _exit(1);
    }

    // parent process
    close(p_write);

    FILE* output = fdopen(p_read, "r");
    int status = 0;

    char buffer[1024];


    while(fgets(buffer, sizeof(buffer), output)) {
        printf("%s", buffer);
    }

    wait(&status);

    if(status != EXIT_SUCCESS) {
        FATAL_FMT("command failed with status %d", status);
        exit(EXIT_FAILURE);
    }

#endif // ROB_POSIX
}

void rob_sync_target(rob_target* target) {
    rob_outdated_targets outdated = rob_target_outdated(target);
    if (!outdated.is_outdated) {
        return;
    }

    for (size_t i = 0; i < outdated.count; i++) {
        rob_sync_target(outdated.items[i]);
    }

    rob_run_command(&target->b);
}

rob_outdated_targets rob_target_outdated(rob_target* t) {
    rob_outdated_targets targets = {0};
    targets.is_outdated = false;

    // printf("inputs: %zu input_target..: %zu\n", t->inputs.count, t->input_target_dependencies.count);
    if (t->inputs.count <= 0 && t ->input_target_dependencies.count <= 0) {
        if (access(t->output, F_OK) != 0) {
            targets.is_outdated = true;
            return targets;
        }
    }

    for (size_t i = 0; i < t->input_target_dependencies.count; i++) {
        rob_target* target_input = t->input_target_dependencies.items[i];
        int result = rob_is_newer_than(target_input->output, t->output);
        switch (result) {
        case true:
        case -1:
        case -2:
            targets.is_outdated = true;
            rob_da_append(&targets, target_input);
            break;
        case false:
            break;
        default:
            assert(false && "rob_is_newer_than returned invalid return value. This could happen because i forgot to update this switch.");
        }
    }

    for (size_t j = 0; j < t->inputs.count; j++) {
        int result = rob_is_newer_than(t->inputs.items[j], t->output);
        switch (result) {
        case true:
        case -1:
        case -2:
            targets.is_outdated = true;
            break;
        case false:
            break;
        default:
            assert(false && "rob_is_newer_than returned invalid return value. This could happen because i forgot to update this switch.");
        }
    }

    return targets;
}

rob_target rob_target_create(rob_command_builder b, const char* output) {
    return (rob_target){
        .output = output,
        .b = b,
        .inputs = {0},
        .input_target_dependencies = {0},
    };
}

void rob__rebuild_self(char* c_file, char* bin_file) {
    if (rob__is_outdated(c_file, bin_file)) {
        INFO("rob is outdated, rebuilding and restarting.");
        rob__compile_self(c_file, bin_file);
        rob_command_builder b = rob_create_command_builder(bin_file);
        rob_run_command(&b);
        exit(EXIT_SUCCESS);
    }
}

bool rob__is_outdated(char* c_file, char* bin_file) {
    return rob__is_newer_than_or_error(c_file, bin_file) || rob__is_newer_than_or_error("rob.h", bin_file);
}
bool rob__is_newer_than_or_error(char* new, char* old) {
    errno = 0;
    int result = rob_is_newer_than(new, old);
    if (result == -1) {
        FATAL_FMT("Could net get file stats for file \"%s\" due to %s.\n", new, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (result == -2) {
        FATAL_FMT("Could net get file stats for file \"%s\" due to %s.\n", old, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

int rob_is_newer_than(const char* new, const char* old) {
#ifdef ROB_POSIX

    struct stat new_file_stat = {0};
    errno = 0;
    int err = stat(new, &new_file_stat);
    if (err != 0) {
        // FATAL_FMT("Could net get file stats for file \"%s\" due to %s.\n", new, strerror(errno));
        // exit(EXIT_FAILURE);
        return -1;
    }

    struct stat old_file_stat = {0};
    errno = 0;
    err = stat(old, &old_file_stat);
    if (err != 0) {
        // FATAL_FMT("Could net get file stats for file \"%s\" due to %s.\n", old, strerror(errno));
        // exit(EXIT_FAILURE);
        return -2;
    }
    // printf("%s : sec %ld nsec %ld\n%s : sec %ld nsec %ld\n", new, new_file_stat.st_mtim.tv_sec, new_file_stat.st_mtim.tv_nsec, old, old_file_stat.st_mtim.tv_sec, old_file_stat.st_mtim.tv_nsec);

    if (new_file_stat.st_mtim.tv_sec != old_file_stat.st_mtim.tv_sec) {
        return new_file_stat.st_mtim.tv_sec > old_file_stat.st_mtim.tv_sec;
    } else {
        return new_file_stat.st_mtim.tv_nsec > old_file_stat.st_mtim.tv_nsec;
    }

#endif // ROB_POSIX
}

void rob__compile_self(char* c_file, char* bin_file) {
    rob_command_builder builder = rob_create_command_builder("gcc");
    rob_command_append(&builder, "-std=gnu99", "-Wall", "-Wextra", c_file, "-o", bin_file);
    rob_run_command(&builder);
}

#endif // ROB_IMPLEMENTATION

#endif // ROB_H_
