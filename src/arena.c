#include "arena.h"


Arena* arena_init(size_t capacity) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) return NULL;
    arena->buffer = (char*)malloc(capacity);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }
    arena->capacity = capacity;
    arena->current_offset = 0;
    return arena;
}

void* arena_alloc(Arena* arena, size_t size) {
    if (arena->current_offset + size > arena->capacity) {
        fprintf(stderr, "Arena out of memory!\n");
        return NULL;
    }
    void* ptr = (void*)(arena->buffer + arena->current_offset);
    arena->current_offset += size;
    return ptr;
}

void arena_free(Arena* arena) {
    if (arena) {
        free(arena->buffer);
        free(arena);
    }
}

void arena_reset(Arena* arena) {
    if (arena) {
        arena->current_offset = 0;
    }
}
