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

    softbody_set_points(softbody, target_shape_position, position);
    softbody_set_velocity(softbody, (Vector2) {0.0f, 0.0f});

    return softbody;
}

void softbody_align_target(
    Softbody* softbody,
    Vector2* target_shape,
    int second_buffer
) {
    unsigned offset = second_buffer * softbody->points;

    float sin_a = sin(softbody->rotation);
    float cos_a = cos(softbody->rotation);
    Vector2 tp = Vector2Zero();

    for (int i = 0; i < softbody->points; i++) {
        Vector2* p = &softbody->shape[i+offset];
        Vector2* pv = &softbody->shape_velocity[i+offset];

        tp.x = target_shape[i].x * cos_a - target_shape[i].y * sin_a;
        tp.y = target_shape[i].x * sin_a + target_shape[i].y * cos_a;

        float tx = softbody->average_position.x + tp.x * softbody->size;
        float ty = softbody->average_position.y + tp.y * softbody->size;

        pv->x += (tx - p->x) * softbody->elasticity;
        pv->y += (ty - p->y) * softbody->elasticity;
    }
}

void softbody_move(
    Softbody* softbody,
    Rectangle* border,
    int second_buffer
) {
    unsigned offset = second_buffer * softbody->points;
    unsigned old_offset = softbody->points - offset;

    softbody->old_average_position.x = softbody->average_position.x;
    softbody->old_average_position.y = softbody->average_position.y;
    softbody->average_position.x = 0;
    softbody->average_position.y = 0;

    // TODO: Remove this just for testing
    softbody->rotation += 0.1;

    // Update points in current buffer
    for (int i = 0; i < softbody->points; i++) {
        Vector2* p = &softbody->shape[i+offset];
        Vector2* pv = &softbody->shape_velocity[i+offset];

        Vector2* op = &softbody->shape[i+old_offset];
        Vector2* opv = &softbody->shape_velocity[i+old_offset];

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
        softbody->average_position.x += p->x;
        softbody->average_position.y += p->y;
    }

    softbody->average_position.x /= softbody->points;
    softbody->average_position.y /= softbody->points;
}

void softbody_set_velocity(
    Softbody* softbody,
    Vector2 velocity
) {
    for (int i = 0; i < softbody->points; i++) {
        softbody->shape_velocity[i].x = velocity.x;
        softbody->shape_velocity[i].y = velocity.y;
        softbody->shape_velocity[i+softbody->points].x = velocity.x;
        softbody->shape_velocity[i+softbody->points].y = velocity.y;
    }
}

void softbody_set_points(
    Softbody* softbody,
    Vector2* target_shape,
    Vector2 pos
) {
    softbody->average_position.x = pos.x;
    softbody->average_position.y = pos.y;
    softbody->old_average_position.x = pos.x;
    softbody->old_average_position.y = pos.y;

    float sin_a = sin(softbody->rotation);
    float cos_a = cos(softbody->rotation);
    Vector2 tp = Vector2Zero();

    for (int i = 0; i < softbody->points; i++) {
        tp.x = target_shape[i].x * cos_a - target_shape[i].y * sin_a;
        tp.y = target_shape[i].x * sin_a + target_shape[i].y * cos_a;

        float tx = softbody->average_position.x + tp.x * softbody->size;
        float ty = softbody->average_position.y + tp.y * softbody->size;
        softbody->shape[i].x = tx;
        softbody->shape[i].y = ty;
        softbody->shape[i+softbody->points].x = tx;
        softbody->shape[i+softbody->points].y = ty;
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
