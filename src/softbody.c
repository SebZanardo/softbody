#include "softbody.h"

void softbody_align_target(
    Softbody* softbody,
    int second_buffer
) {
    unsigned offset = second_buffer * softbody->points;

    for (int i = 0; i < softbody->points; i++) {
        Vector2* p = &softbody->shape[i+offset];
        Vector2* pv = &softbody->shape_velocity[i+offset];
        float tx = softbody->average_position.x + softbody->target_shape[i].x;
        float ty = softbody->average_position.y + softbody->target_shape[i].y;

        pv->x += (tx - p->x) * DAMPING;
        pv->y += (ty - p->y) * DAMPING;
    }
}

void softbody_move(
    Softbody* softbody,
    Rectangle border,
    int second_buffer
) {
    unsigned offset = second_buffer * softbody->points;
    unsigned old_offset = softbody->points - offset;

    softbody->old_average_position.x = softbody->average_position.x;
    softbody->old_average_position.y = softbody->average_position.y;
    softbody->average_position.x = 0;
    softbody->average_position.y = 0;

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

        p->x = op->x + pv->x;
        p->y = op->y + pv->y;

        // Clamp points to bounds
        if (p->x < border.x) {
            p->x = border.x;
            pv->x = 0.0;
        } else if (p->x > border.x + border.width) {
            p->x = border.x + border.width;
            pv->x = 0.0;
        }
        if (p->y < border.y) {
            p->y = border.y;
            pv->y = 0.0;
        } else if (p->y > border.y + border.height) {
            p->y = border.y + border.height;
            pv->y = 0.0;
        }
        softbody->average_position.x += p->x;
        softbody->average_position.y += p->y;
    }

    softbody->average_position.x /= softbody->points;
    softbody->average_position.y /= softbody->points;
}

void softbody_set_velocity(Softbody* softbody, Vector2 velocity) {
    for (int i = 0; i < softbody->points; i++) {
        softbody->shape_velocity[i].x = velocity.x;
        softbody->shape_velocity[i].y = velocity.y;
        softbody->shape_velocity[i+softbody->points].x = velocity.x;
        softbody->shape_velocity[i+softbody->points].y = velocity.y;
    }
}

void softbody_set_points(Softbody* softbody, Vector2 position) {
    softbody->average_position.x = position.x;
    softbody->average_position.y = position.y;
    softbody->old_average_position.x = position.x;
    softbody->old_average_position.y = position.y;
    for (int i = 0; i < softbody->points; i++) {
        softbody->shape[i].x = position.x + softbody->target_shape[i].x;
        softbody->shape[i].y = position.y + softbody->target_shape[i].y;
        softbody->shape[i+softbody->points].x = position.x + softbody->target_shape[i].x;
        softbody->shape[i+softbody->points].y = position.y + softbody->target_shape[i].y;
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

Softbody* softbody_init(
    Arena* arena,
    unsigned points,
    Vector2 position,
    float radius
) {
    Softbody* softbody = (Softbody*)arena_alloc(arena, sizeof(Softbody));
    if (!softbody) return NULL;

    Vector2* target_shape = (Vector2*)arena_alloc(arena, sizeof(Vector2) * points);
    if (!target_shape) return NULL;

    Vector2* shape = (Vector2*)arena_alloc(arena, sizeof(Vector2) * points * 2);
    if (!shape) return NULL;

    Vector2* shape_velocity = (Vector2*)arena_alloc(arena, sizeof(Vector2) * points * 2);
    if (!shape_velocity) return NULL;

    softbody->target_shape = target_shape;
    softbody->shape = shape;
    softbody->shape_velocity = shape_velocity;
    softbody->average_position = position;
    softbody->old_average_position = position;
    softbody->points = points;
    softbody->radius = radius;

    centred_polygon(target_shape, points, radius);
    softbody_set_points(softbody, position);
    softbody_set_velocity(softbody, (Vector2) {0.0f, 0.0f});

    return softbody;
}

SlimeVisual* softbody_visual_init(
    Arena* arena,
    Color eye_colour,
    Color body_colour,
    unsigned eyes,
    float max_radius
) {
    SlimeVisual* slime_visual = (SlimeVisual*)arena_alloc(arena, sizeof(SlimeVisual));
    if (!slime_visual) return NULL;

    Vector2* eye_positions = (Vector2*)arena_alloc(arena, sizeof(Vector2) * eyes);
    if (!eye_positions) return NULL;

    EyeTypes* eye_types = (EyeTypes*)arena_alloc(arena, sizeof(EyeTypes) * eyes);
    if (!eye_types) return NULL;


    for (int i = 0; i < eyes; i++) {
        float random = (double)rand() / RAND_MAX;

        // Calculate random position inside of the max_radius
        float theta = random * 2 * PI;
        float r = max_radius * 0.75f * sqrt(random);
        float x = r * cos(theta);
        float y = r * sin(theta);

        eye_positions[i] = (Vector2) {x, y};
        eye_types[i] = GetRandomValue(0, EYETYPE_COUNT - 1);
    }

    slime_visual->eye_positions = eye_positions;
    slime_visual->eye_types = eye_types;
    slime_visual->eye_colour = eye_colour;
    slime_visual->body_colour = body_colour;
    slime_visual->eyes = eyes;

    return slime_visual;
}

void softbody_create_random(
    Arena* arena,
    Softbody** softbodies,
    SlimeVisual** slime_visuals,
    unsigned* active_softbodies
) {
    if (*active_softbodies == MAX_SOFTBODIES) {
        printf("At softbody capacity\n");
        return;
    }

    float radius = GetRandomValue(50, 100);

    Softbody* softbody = softbody_init(
        arena,
        GetRandomValue(10, MAX_POINTS),
        (Vector2) {GetRandomValue(0, WINDOW_WIDTH), GetRandomValue(0, WINDOW_HEIGHT)},
        radius
    );

    SlimeVisual* slime_visual = softbody_visual_init(
        arena,
        (Color) {GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255},
        (Color) {GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 200},
        GetRandomValue(1, 8),
        radius
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
