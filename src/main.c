#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

// TODO: Move all these #defines to better spots
#define POINTS 20
#define BUFFER_POINTS (POINTS * 2)
#define STEP_AMOUNT (PI * 2 / POINTS)
#define RADIUS 50
#define RADIUS_SQR (RADIUS * RADIUS)

#define GRAVITY_X 0.0
#define GRAVITY_Y 0.5
#define BORDER 20
#define DAMPING 0.9
#define FRICTION 0.95
#define MAX_VEL 10


void softbody_set_points(
    Vector2 shape[BUFFER_POINTS],
    Vector2 target_shape[POINTS],
    float cx,
    float cy
);
void softbody_set_velocity(
    Vector2 shape_velocity[BUFFER_POINTS], float vx, float vy
);
Vector2 softbody_move(
    Vector2 shape[BUFFER_POINTS],
    Vector2 shape_velocity[BUFFER_POINTS],
    Rectangle border,
    unsigned offset,
    unsigned old_offset
);
void softbody_align_target(
    Vector2 shape[BUFFER_POINTS],
    Vector2 shape_velocity[BUFFER_POINTS],
    Vector2 target_shape[BUFFER_POINTS],
    Vector2 average_pos,
    unsigned offset
);
void centred_polygon(
    Vector2 target_shape[POINTS],
    Vector2 target_shape_dist[POINTS]
);
unsigned modulo(int value, unsigned m);


int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "softbody");
    SetTargetFPS(60);

    HideCursor();

    SetRandomSeed(0);

    Vector2 target_shape[POINTS] = {0};
    Vector2 target_shape_dist[POINTS] = {0};
    centred_polygon(target_shape, target_shape_dist);

    Vector2 shape[BUFFER_POINTS] = {0};
    Vector2 shape_velocity[BUFFER_POINTS] = {0};

    float cx = (float)WINDOW_WIDTH / 2;
    float cy = (float)WINDOW_HEIGHT / 2;
    softbody_set_points(shape, target_shape, cx, cy);

    // TODO: Store angular rotation and velocity somewhere
    // In struct with target shape and dist

    unsigned offset = POINTS;
    unsigned old_offset = 0;

    Rectangle border = {
        BORDER,
        BORDER,
        WINDOW_WIDTH - BORDER * 2,
        WINDOW_HEIGHT - BORDER * 2
    };

    Vector2 mouse_pos = {0};
    Vector2 old_mouse_pos = {0};

    Vector2 average_pos = {};

    while (!WindowShouldClose()) {
        old_mouse_pos = mouse_pos;
        mouse_pos = GetMousePosition();
        float mx = mouse_pos.x;
        float my = mouse_pos.y;

        // Flip offset
        old_offset = offset;
        offset = offset == 0 ? POINTS : 0;

        // Check if should drag points and reform
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float dx = mouse_pos.x - old_mouse_pos.x;
            float dy = mouse_pos.y - old_mouse_pos.y;
            average_pos.x = mx;
            average_pos.y = my;
            softbody_set_points(shape, target_shape, mx, my);
            softbody_set_velocity(shape_velocity, dx, dy);
        } else {
            // Move and calculate centre point
            average_pos = softbody_move(
                shape, shape_velocity, border, offset, old_offset
            );

            // Apply spring force to points
            for (int i = 0; i < POINTS; i++) {
                unsigned last_index = modulo(i-1, POINTS)+offset;
                unsigned next_index = modulo(i+1, POINTS)+offset;

                Vector2* lp = &shape[last_index];
                Vector2* lpv = &shape_velocity[last_index];
                Vector2* lt = &target_shape[last_index];

                Vector2* p = &shape[i+offset];
                Vector2* pv = &shape_velocity[i+offset];
                Vector2* t = &target_shape[i];

                Vector2* np = &shape[next_index];
                Vector2* npv = &shape_velocity[next_index];
                Vector2* nt = &target_shape[next_index];
            }

            // Apply force to pull or retract points to target
            softbody_align_target(
                shape, shape_velocity, target_shape, average_pos, offset);
        }

        // RENDER
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleLinesEx(border, 2, WHITE);

        DrawCircleV(mouse_pos, 8, MAGENTA);

        for (int i = 0; i < POINTS; i++) {
            DrawCircleV(shape[i+offset], 4, GREEN);
        }

        DrawCircleV(average_pos, 8, YELLOW);
        EndDrawing();
    }
}

void softbody_set_points(
    Vector2 shape[BUFFER_POINTS],
    Vector2 target_shape[POINTS],
    float cx,
    float cy
) {
    for (int i = 0; i < POINTS; i++) {
        shape[i].x = cx + target_shape[i].x;
        shape[i].y = cy + target_shape[i].y;
        shape[i+POINTS].x = cx + target_shape[i].x;
        shape[i+POINTS].y = cy + target_shape[i].y;
    }
}

void softbody_set_velocity(
    Vector2 shape_velocity[BUFFER_POINTS], float vx, float vy
) {
    for (int i = 0; i < POINTS; i++) {
        shape_velocity[i].x = vx;
        shape_velocity[i].y = vy;
        shape_velocity[i+POINTS].x = vx;
        shape_velocity[i+POINTS].y = vy;
    }
}

Vector2 softbody_move(
    Vector2 shape[BUFFER_POINTS],
    Vector2 shape_velocity[BUFFER_POINTS],
    Rectangle border,
    unsigned offset,
    unsigned old_offset
) {
    Vector2 average_pos = {0};

    // Update points in current buffer
    for (int i = 0; i < POINTS; i++) {
        Vector2* p = &shape[i+offset];
        Vector2* pv = &shape_velocity[i+offset];

        Vector2* op = &shape[i+old_offset];
        Vector2* opv = &shape_velocity[i+old_offset];

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
        average_pos.x += p->x;
        average_pos.y += p->y;
    }

    average_pos.x /= POINTS;
    average_pos.y /= POINTS;

    return average_pos;
}

void softbody_align_target(
    Vector2 shape[BUFFER_POINTS],
    Vector2 shape_velocity[BUFFER_POINTS],
    Vector2 target_shape[BUFFER_POINTS],
    Vector2 average_pos,
    unsigned offset
) {
    for (int i = 0; i < POINTS; i++) {
        Vector2* p = &shape[i+offset];
        Vector2* pv = &shape_velocity[i+offset];
        float tx = average_pos.x + target_shape[i].x;
        float ty = average_pos.y + target_shape[i].y;

        pv->x += (tx - p->x) * DAMPING;
        pv->y += (ty - p->y) * DAMPING;
    }
}

void centred_polygon(
    Vector2 target_shape[POINTS],
    Vector2 target_shape_dist[POINTS]
) {
    for (int i = 0; i < POINTS; i++) {
        float x = cos(STEP_AMOUNT * i) * RADIUS;
        float y = sin(STEP_AMOUNT * i) * RADIUS;

        target_shape[i].x = x;
        target_shape[i].y = y;

        unsigned next_index = modulo(i + 1, POINTS);
        float nx = cos(STEP_AMOUNT * next_index) * RADIUS;
        float ny = sin(STEP_AMOUNT * next_index) * RADIUS;

        // Store distance to next point in second buffer
        target_shape_dist[i].x = nx - x;
        target_shape_dist[i].y = ny - y;
    }
}

unsigned modulo(int value, unsigned m) {
    int mod = value % (int)m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}
