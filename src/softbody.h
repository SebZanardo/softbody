#ifndef SOFTBODY_H
#define SOFTBODY_H


#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "arena.h"
#include "constants.h"


typedef enum {
    SMALL,
    MEDIUM,
    EYETYPE_NONE,
    EYETYPE_COUNT
} EyeTypes;

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

typedef struct {
    EyeTypes* eye_types;
    Color eye_colour;
    Color body_colour;
} SlimeVisual;


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
SlimeVisual* slime_visual_init(
    Arena* arena,
    Color eye_colour,
    Color body_colour,
    unsigned eyes
);
void softbody_create_random(
    Arena* arena,
    Softbody** softbodies,
    SlimeVisual** slime_visuals,
    Vector2** target_shape_positions,
    unsigned* target_shape_points,
    unsigned* active_softbodies
);
void softbody_align_target(
    Vector2* target_shape,
    Vector2* shape,
    Vector2* shape_velocity,
    Vector2* average_position,
    unsigned points,
    float size,
    float elasticity,
    int second_buffer
);
void softbody_move(
    Vector2* shape,
    Vector2* shape_velocity,
    Rectangle* border,
    Vector2* old_average_position,
    Vector2* average_position,
    unsigned points,
    int second_buffer
);
void softbody_set_velocity(
    Vector2* shape_velocity,
    Vector2 velocity,
    unsigned points
);
void softbody_set_points(
    Vector2* target_shape,
    Vector2* shape,
    Vector2* average_position,
    Vector2* old_average_position,
    Vector2 position,
    unsigned points,
    float size
);
void centred_polygon(Vector2* target_shape, unsigned points, float radius);


#endif  /* SOFTBODY_H */
