#ifndef ARENA_H
#define ARENA_H

#include "stdio.h"
#include "stdlib.h"


typedef struct {
    char* buffer;
    size_t capacity;
    size_t current_offset;
} Arena;


Arena* arena_init(size_t capacity);
void* arena_alloc(Arena* arena, size_t size);
void arena_free(Arena* arena);
void arena_reset(Arena* arena);

#endif  /* ARENA_H */
