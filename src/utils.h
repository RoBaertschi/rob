#ifndef ROB_UTILS_H_
#define ROB_UTILS_H_

#include <stdbool.h>
#include <stdlib.h>

// ================
// ------ da ------
// da is a dynamic array, it should contain the following members:
// *items   - A pointer to a item
// count    - A integer type for the size, often a size_t
// capacity - A integer type for the capacity, often a size_t
// ================

#define da_initial_cap 4

/// Returns the new capacity
#define da_grow(da, item_size) (\
        (da)->items != NULL \
            ? ( (da)->items = realloc( (da)->items, item_size * (da)->capacity * 2 ) )\
                == NULL ? \
                    ((da)->capacity = 0, (da)->count = 0) \
                    : ((da)->capacity *= 2)\
            : ( ( (da)->items = malloc(da_initial_cap * item_size) ) == NULL\
                ? 0\
                : ((da)->capacity = da_initial_cap)\
            )\
        )

#define da_ensure_size(da, size, item_size) \
    while((da)->capacity < size) {\
        da_grow((da), item_size);\
    }


#define da_append(da, item) {da_ensure_size(( da ), ( da )->count + 1, sizeof( item ) );\
                            ((da)->items[((da)->count)++] = item);\
}

#define da_pop(da) {((da)->items[((da)->count = ((da)->count - 1))]);}

#define da_foreach(da, type, elementname) size_t i = 0; for (type elementname; i < (da)->count; i++, (elementname = (da)->items[i]))

// ================
// ------ sp ------
// sp is a facility to manage strings. sp allocates a big space for static strings.
// This allows for cleanup of all the strings later on.
// ================

typedef size_t str;

void sp_init(void);
void sp_destroy(void);
/// Allocates a string using the format and returns a pointer to it.
char* s(const char* fmt, ...);

#endif // ROB_UTILS_H_
