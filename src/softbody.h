#ifndef SOFTBODY_H
#define SOFTBODY_H

#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "arena.h"
#include "constants.h"

typedef struct {
    Vector2* target_shape;
    Vector2* shape;
    Vector2* shape_velocity;
    Vector2 average_position;
    Vector2 old_average_position;
    Color eye_colour;
    Color body_colour;
    unsigned points;
    float radius;
} Softbody;

void softbody_align_target(
    Softbody* softbody,
    int second_buffer
);

void softbody_move(
    Softbody* softbody,
    Rectangle border,
    int second_buffer
);

void softbody_set_velocity(Softbody* softbody, Vector2 velocity);

void softbody_set_points(Softbody* softbody, Vector2 position);

void centred_polygon(Vector2* target_shape, unsigned points, float radius);

Softbody* softbody_init(
    Arena* arena,
    unsigned points,
    Vector2 position,
    float radius,
    Color eye_colour,
    Color body_colour
);

void softbody_create_random(
    Arena* arena,
    Softbody** softbodies,
    unsigned* active_softbodies
);

#endif  /* SOFTBODY_H */
