#include "slime.h"


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

void slime_create_random(
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
    float size = GetRandomValue(5, 10);
    float elasticity = (double) rand() / RAND_MAX;
    elasticity = Clamp(elasticity, 0.3f, 0.7f);
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
        elasticity,
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
