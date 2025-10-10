#include "softbody.h"


Softbody* softbody_init(
    Arena* arena,
    Vector2* target_shape_position,
    Vector2 position,
    unsigned points,
    float size,
    float mass,
    float elasticity,
    Shapes target_shape
) {
    Softbody* softbody = (Softbody*)arena_alloc(arena, sizeof(Softbody));
    if (!softbody) return NULL;

    Vector2* shape = (Vector2*)arena_alloc(arena, sizeof(Vector2) * points * 2);
    if (!shape) return NULL;

    Vector2* shape_velocity = (Vector2*)arena_alloc(arena, sizeof(Vector2) * points * 2);
    if (!shape_velocity) return NULL;

    softbody->shape = shape;
    softbody->shape_velocity = shape_velocity;
    softbody->average_position = position;
    softbody->old_average_position = position;
    softbody->rotation_velocity = 0.0f;
    softbody->rotation = 0.0f;
    softbody->old_rotation = 0.0f;
    softbody->size = size;
    softbody->mass = mass;
    softbody->elasticity = elasticity;
    softbody->points = points;
    softbody->target_shape = target_shape;

    softbody_set_points(
        target_shape_position,
        softbody->shape,
        &softbody->average_position,
        &softbody->old_average_position,
        position,
        softbody->points,
        softbody->size
    );
    softbody_set_velocity(
        softbody->shape_velocity, (Vector2) {0.0f, 0.0f}, softbody->points
    );

    return softbody;
}

SlimeVisual* slime_visual_init(
    Arena* arena,
    Color eye_colour,
    Color body_colour,
    unsigned eyes
) {
    SlimeVisual* slime_visual = (SlimeVisual*)arena_alloc(arena, sizeof(SlimeVisual));
    if (!slime_visual) return NULL;

    EyeTypes* eye_types = (EyeTypes*)arena_alloc(arena, sizeof(EyeTypes) * eyes);
    if (!eye_types) return NULL;

    for (int i = 0; i < eyes; i++) {
        eye_types[i] = GetRandomValue(0, EYETYPE_COUNT - 1);
    }

    slime_visual->eye_types = eye_types;
    slime_visual->eye_colour = eye_colour;
    slime_visual->body_colour = body_colour;

    return slime_visual;
}

void softbody_create_random(
    Arena* arena,
    Softbody** softbodies,
    SlimeVisual** slime_visuals,
    Vector2** target_shape_positions,
    unsigned* target_shape_points,
    unsigned* active_softbodies
) {
    if (*active_softbodies == MAX_SOFTBODIES) {
        printf("At softbody capacity\n");
        return;
    }

    Shapes target_shape = GetRandomValue(0, SHAPES_COUNT - 1);
    float size = GetRandomValue(3, 10);
    Vector2 position = (Vector2) {
        GetRandomValue(0, WINDOW_WIDTH),
        GetRandomValue(0, WINDOW_HEIGHT)
    };
    unsigned points = target_shape_points[target_shape];

    Softbody* softbody = softbody_init(
        arena,
        target_shape_positions[target_shape],
        position,
        points,
        size,
        1.0f,
        0.1f,
        target_shape
    );

    SlimeVisual* slime_visual = slime_visual_init(
        arena,
        (Color) {
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            255
        },
        (Color) {
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            200
        },
        points + 1
    );

    if (!softbody || !slime_visual) {
        printf("Failed to instantiate\n");
        return;
    }

    softbodies[*active_softbodies] = softbody;
    slime_visuals[*active_softbodies] = slime_visual;

    (*active_softbodies)++;

    printf("Created!\n");
}

void softbody_align_target(
    Vector2* target_shape,
    Vector2* shape,
    Vector2* shape_velocity,
    Vector2* average_position,
    unsigned points,
    float size,
    float elasticity,
    int second_buffer
) {
    unsigned offset = second_buffer * points;

    for (int i = 0; i < points; i++) {
        Vector2* p = &shape[i+offset];
        Vector2* pv = &shape_velocity[i+offset];
        float tx = average_position->x + target_shape[i].x * size;
        float ty = average_position->y + target_shape[i].y * size;

        pv->x += (tx - p->x) * elasticity;
        pv->y += (ty - p->y) * elasticity;
    }
}

void softbody_move(
    Vector2* shape,
    Vector2* shape_velocity,
    Rectangle* border,
    Vector2* old_average_position,
    Vector2* average_position,
    unsigned points,
    int second_buffer
) {
    unsigned offset = second_buffer * points;
    unsigned old_offset = points - offset;

    old_average_position->x = average_position->x;
    old_average_position->y = average_position->y;
    average_position->x = 0;
    average_position->y = 0;

    // Update points in current buffer
    for (int i = 0; i < points; i++) {
        Vector2* p = &shape[i+offset];
        Vector2* pv = &shape_velocity[i+offset];

        Vector2* op = &shape[i+old_offset];
        Vector2* opv = &shape_velocity[i+old_offset];

        pv->x = (opv->x + GRAVITY_X) * FRICTION;
        pv->y = (opv->y + GRAVITY_Y) * FRICTION;

        Clamp(pv->x, -MAX_VEL, MAX_VEL);
        Clamp(pv->y, -MAX_VEL, MAX_VEL);

        p->x = (op->x + pv->x);
        p->y = (op->y + pv->y);

        // Clamp points to bounds
        if (p->x < border->x) {
            p->x = border->x;
            pv->x = 0.0;
        } else if (p->x > border->x + border->width) {
            p->x = border->x + border->width;
            pv->x = 0.0;
        }
        if (p->y < border->y) {
            p->y = border->y;
            pv->y = 0.0;
        } else if (p->y > border->y + border->height) {
            p->y = border->y + border->height;
            pv->y = 0.0;
        }
        average_position->x += p->x;
        average_position->y += p->y;
    }

    average_position->x /= points;
    average_position->y /= points;
}

void softbody_set_velocity(
    Vector2* shape_velocity,
    Vector2 velocity,
    unsigned points
) {
    for (int i = 0; i < points; i++) {
        shape_velocity[i].x = velocity.x;
        shape_velocity[i].y = velocity.y;
        shape_velocity[i+points].x = velocity.x;
        shape_velocity[i+points].y = velocity.y;
    }
}

void softbody_set_points(
    Vector2* target_shape,
    Vector2* shape,
    Vector2* average_position,
    Vector2* old_average_position,
    Vector2 position,
    unsigned points,
    float size
) {
    average_position->x = position.x;
    average_position->y = position.y;
    old_average_position->x = position.x;
    old_average_position->y = position.y;
    for (int i = 0; i < points; i++) {
        shape[i].x = position.x + target_shape[i].x * size;
        shape[i].y = position.y + target_shape[i].y * size;
        shape[i+points].x = position.x + target_shape[i].x * size;
        shape[i+points].y = position.y + target_shape[i].y * size;
    }
}

void centred_polygon(Vector2* target_shape, unsigned points, float radius) {
    float step_amount = 2 * PI / points;
    for (int i = 0; i < points; i++) {
        float x = cos(step_amount * i) * radius;
        float y = sin(step_amount * i) * radius;
        target_shape[i].x = x;
        target_shape[i].y = y;
    }
}
