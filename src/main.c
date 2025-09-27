#include "raylib.h"
#include <math.h>
#include <stdio.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define POINTS 20
#define BUFFER_POINTS (POINTS * 2)
#define STEP_AMOUNT (PI * 2 / POINTS)
#define RADIUS 50
#define RADIUS_SQR (RADIUS * RADIUS)

#define GRAVITY_X 0.0
#define GRAVITY_Y 0.5
#define BORDER 20
#define DAMPING 0.5
#define FRICTION 0.99
#define MAX_VEL 10


void reset_shape(
    Vector2 shape[BUFFER_POINTS],
    Vector2 target_shape[POINTS],
    double cx,
    double cy
);
void set_shape_velocity(Vector2 shape_velocity[BUFFER_POINTS], double vx, double vy);


int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "softbody");
    SetTargetFPS(60);

    HideCursor();

    SetRandomSeed(0);

    // Set target shape
    Vector2 target_shape[POINTS] = {0};

    // N-sided polygon maths
    for (int i = 0; i < POINTS; i++) {
        double dx = cos(STEP_AMOUNT * i);
        double dy = sin(STEP_AMOUNT * i);
        target_shape[i].x = dx * RADIUS;
        target_shape[i].y = dy * RADIUS;
    }

    // Double buffer for accurate simulation
    Vector2 shape[BUFFER_POINTS] = {0};
    Vector2 shape_velocity[BUFFER_POINTS] = {0};

    double cx = (double)WINDOW_WIDTH / 2;
    double cy = (double)WINDOW_HEIGHT / 2;
    reset_shape(shape, target_shape, cx, cy);

    // TODO: Store angular rotation and velocity somewhere

    unsigned int offset = POINTS;
    unsigned int old_offset = 0;

    Rectangle border = {
        BORDER,
        BORDER,
        WINDOW_WIDTH - BORDER * 2,
        WINDOW_HEIGHT - BORDER * 2
    };

    Vector2 mouse_pos = {0};
    Vector2 old_mouse_pos = {0};

    double ax = 0.0;
    double ay = 0.0;

    while (!WindowShouldClose()) {
        old_mouse_pos = mouse_pos;
        mouse_pos = GetMousePosition();
        double mx = mouse_pos.x;
        double my = mouse_pos.y;

        // Flip offset
        old_offset = offset;
        offset = offset == 0 ? POINTS : 0;

        // Check if should drag points and reform
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            double dx = mouse_pos.x - old_mouse_pos.x;
            double dy = mouse_pos.y - old_mouse_pos.y;
            ax = mx;
            ay = my;
            reset_shape(shape, target_shape, mx, my);
            set_shape_velocity(shape_velocity, dx, dy);
        } else {
            ax = 0.0;
            ay = 0.0;
            // Update points in current buffer
            for (int i = 0; i < POINTS; i++) {
                Vector2* p = &shape[i+offset];
                Vector2* pv = &shape_velocity[i+offset];
                Vector2* op = &shape[i+old_offset];
                Vector2* opv = &shape_velocity[i+old_offset];

                pv->x = (opv->x + GRAVITY_X) * FRICTION;
                pv->y = (opv->y + GRAVITY_Y) * FRICTION;

                if (pv->x > MAX_VEL) {
                    pv->x = MAX_VEL;
                } else if (pv->x < -MAX_VEL) {
                    pv->x = -MAX_VEL;
                }

                if (pv->y > MAX_VEL) {
                    pv->y = MAX_VEL;
                } else if (pv->y < -MAX_VEL) {
                    pv->y = -MAX_VEL;
                }

                p->x = op->x + pv->x;
                p->y = op->y + pv->y;
            }

            // Clamp points to bounds
            for (int i = 0; i < POINTS; i++) {
                Vector2* p = &shape[i+offset];
                Vector2* pv = &shape_velocity[i+offset];
                if (p->x < border.x) {
                    p->x = border.x;
                    pv->x = 0.0;
                }
                else if (p->x > border.x + border.width) {
                    p->x = border.x + border.width;
                    pv->x = 0.0;
                }
                if (p->y < border.y) {
                    p->y = border.y;
                    pv->y = 0.0;
                }
                else if (p->y > border.y + border.height) {
                    p->y = border.y + border.height;
                    pv->y = 0.0;
                }
                ax += p->x;
                ay += p->y;
            }

            // Calculate centre point
            ax /= POINTS;
            ay /= POINTS;

            // Apply force to pull or retract points
            for (int i = 0; i < POINTS; i++) {
                Vector2* p = &shape[i+offset];
                Vector2* pv = &shape_velocity[i+offset];
                double tx = ax + target_shape[i].x;
                double ty = ay + target_shape[i].y;

                pv->x += (tx - p->x) * DAMPING;
                pv->y += (ty - p->y) * DAMPING;
            }
        }


        // RENDER
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleLinesEx(border, 2, WHITE);

        DrawCircleV(mouse_pos, 8, MAGENTA);

        for (int i = 0; i < POINTS; i++) {
            DrawCircleV(shape[i+offset], 4, GREEN);
        }

        DrawCircle(ax, ay, 8, YELLOW);
        EndDrawing();
    }
}

void reset_shape(
    Vector2 shape[BUFFER_POINTS],
    Vector2 target_shape[POINTS],
    double cx,
    double cy
) {
    for (int i = 0; i < POINTS; i++) {
        shape[i].x = cx + target_shape[i].x;
        shape[i].y = cy + target_shape[i].y;
        shape[i+POINTS].x = cx + target_shape[i].x;
        shape[i+POINTS].y = cy + target_shape[i].y;
    }
}

void set_shape_velocity(Vector2 shape_velocity[BUFFER_POINTS], double vx, double vy) {
    for (int i = 0; i < POINTS; i++) {
        shape_velocity[i].x = vx;
        shape_velocity[i].y = vy;
        shape_velocity[i+POINTS].x = vx;
        shape_velocity[i+POINTS].y = vy;
    }
}
