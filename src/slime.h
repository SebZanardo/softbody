#ifndef SLIME_H
#define SLIME_H


#include "raylib.h"
#include "arena.h"
#include "softbody.h"


typedef enum {
    SMALL,
    MEDIUM,
    EYETYPE_NONE,
    EYETYPE_COUNT
} EyeTypes;


typedef struct {
    EyeTypes* eye_types;
    Color eye_colour;
    Color body_colour;
} SlimeVisual;


SlimeVisual* slime_visual_init(
    Arena* arena,
    Color eye_colour,
    Color body_colour,
    unsigned eyes
);
void slime_create_random(
    Arena* arena,
    Softbody** softbodies,
    SlimeVisual** slime_visuals,
    Vector2** target_shape_positions,
    unsigned* target_shape_points,
    unsigned* active_softbodies
);


#endif  /* SLIME_H */
