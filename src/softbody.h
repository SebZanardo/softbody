#ifndef SOFTBODY_H
#define SOFTBODY_H


#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "arena.h"
#include "constants.h"


typedef enum {
    TRIANGLE,
    SQUARE,
    PENTAGON,
    OCTAGON,
    SHAPES_COUNT
} Shapes;

typedef struct {
    Vector2* shape;
    Vector2* shape_velocity;
    Vector2 average_position;
    Vector2 old_average_position;
    float rotation_velocity;
    float rotation;
    float old_rotation;
    float size;
    float mass;
    float elasticity;
    Shapes target_shape;
    unsigned points;
} Softbody;

Softbody* softbody_init(
    Arena* arena,
    Vector2* target_shape_position,
    Vector2 position,
    unsigned points,
    float size,
    float mass,
    float elasticity,
    Shapes target_shape
);
void softbody_align_target(
    Softbody* softbody,
    Vector2* target_shape,
    int second_buffer
);
void softbody_move(
    Softbody* softbody,
    Rectangle* border,
    int second_buffer
);
void softbody_set_velocity(
    Softbody* softbody,
    Vector2 velocity
);
void softbody_set_points(
    Softbody* softbody,
    Vector2* target_shape,
    Vector2 position
);
void centred_polygon(
    Vector2* target_shape,
    unsigned points,
    float radius
);


#endif  /* SOFTBODY_H */
