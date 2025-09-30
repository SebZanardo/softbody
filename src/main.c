#include <stdio.h>
#include "arena.h"
#include "constants.h"
#include "raylib.h"
#include "softbody.h"


#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


typedef struct {
    float screen_width;
    float screen_height;
    float scale;
    float offset_x;
    float offset_y;
} WindowParameters;

#ifdef WEB
#include <emscripten/emscripten.h>

EM_JS(bool, IsMobile, (), {
    return (
        typeof navigator !== 'undefined' &&
        /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i
            .test(navigator.userAgent)
    );
});
#endif


void handle_resize();
unsigned modulo(int value, unsigned m);


WindowParameters window;
bool handheld;


int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "softbody");
    SetTargetFPS(60);

    HideCursor();

    SetRandomSeed(1);

    #ifdef WEB
        handheld = IsMobile();
    #endif
    handle_resize();

    /*Image icon = LoadImage("src/resources/icon.png");*/
    /*SetWindowIcon(icon);*/

    RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Temporary
    Texture2D eye_16x16 = LoadTexture("src/data/images/eye(16x16).png");
    Texture2D pupil_16x16 = LoadTexture("src/data/images/pupil(16x16).png");

    Arena* main_arena = arena_init(ARENA_BYTES);
    if (!main_arena) return 1;

    unsigned active_softbodies = 0;
    Softbody* softbodies[MAX_SOFTBODIES];

    Rectangle border = {
        BORDER,
        BORDER,
        WINDOW_WIDTH - BORDER * 2,
        WINDOW_HEIGHT - BORDER * 2
    };

    Vector2 mouse_pos = {0};
    Vector2 old_mouse_pos = {0};

    int second_buffer = 0;

    int hovered = -1;
    int holding = -1;

    softbody_create_random(main_arena, softbodies, &active_softbodies);

    while (!WindowShouldClose()) {
        if (IsWindowResized()) handle_resize();

        Vector2 render_mouse_pos = GetMousePosition();

        #ifdef WEB
            if (!handheld) {
                render_mouse_pos.x *= window.screen_width / WINDOW_WIDTH;
                render_mouse_pos.y *= window.screen_height / WINDOW_HEIGHT;
            }
        #endif

        old_mouse_pos = mouse_pos;
        mouse_pos.x = (render_mouse_pos.x - window.offset_x) / window.scale;
        mouse_pos.y = (render_mouse_pos.y - window.offset_y) / window.scale;

        second_buffer = second_buffer == 1 ? 0 : 1;

        if (IsKeyPressed(KEY_SPACE)) {
            softbody_create_random(main_arena, softbodies, &active_softbodies);
        }

        hovered = -1;
        for (int i = 0; i < active_softbodies; i++) {
            Softbody* softbody = softbodies[i]; 
            if (CheckCollisionPointPoly(mouse_pos, softbody->shape, softbody->points)) {
                hovered = i;
                break;
            }
        }

        // Check if should drag points and reform
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 velocity = {
                mouse_pos.x - old_mouse_pos.x,
                mouse_pos.y - old_mouse_pos.y
            };
            if (holding < 0) {
                holding = hovered;
            }

            if (holding >= 0) {
                Softbody* softbody = softbodies[holding]; 
                softbody_set_points(softbody, mouse_pos);
                softbody_set_velocity(softbody, velocity);
            }
        } else {
            holding = -1;
        }

        for (int i = 0; i < active_softbodies; i++) {
            Softbody* softbody = softbodies[i]; 
            softbody_move(softbody, border, second_buffer);
            softbody_align_target(softbody, second_buffer);
        }

        // RENDER
        BeginTextureMode(target);
        ClearBackground(BLACK);

        DrawRectangleLinesEx(border, 2, WHITE);

        Vector2 render_points[MAX_POINTS] = {0};

        // Draw in reverse order because pickup priority is oldest to newest
        for (int i = active_softbodies - 1; i >= 0; i--) {
            Softbody* softbody = softbodies[i]; 
            unsigned offset = second_buffer*softbody->points;
            for (int p = 0; p < softbody->points; p++) {
                // Invert so points go counter-clockwise
                render_points[softbody->points-p-1] = softbody->shape[p+offset];

                /*DrawCircleV(softbody->shape[p+offset], 4, WHITE);*/
            }
            DrawTriangleFan(render_points, softbody->points, softbody->body_colour);
            DrawTexture(eye_16x16, softbody->average_position.x - 8, softbody->average_position.y - 8, WHITE);
            DrawTexture(pupil_16x16, softbody->average_position.x - 8, softbody->average_position.y - 8, softbody->eye_colour);

            if (holding == i || (holding < 0 && hovered == i)) {
                for (int i = 0; i < softbody->points; i++)
                {
                    DrawLineEx(softbody->shape[i+offset], softbody->shape[modulo(i + 1, softbody->points)+offset], 2.0f, WHITE);
                }
            }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            target.texture,
            (Rectangle){
                0,
                0,
                (float)target.texture.width,
                (float)-target.texture.height
            },
            (Rectangle){
                window.offset_x,
                window.offset_y,
                (float)target.texture.width * window.scale,
                (float)-target.texture.height * window.scale
            },
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        DrawCircleV(render_mouse_pos, 16, MAGENTA);
        EndDrawing();
    }
}

unsigned modulo(int value, unsigned m) {
    int mod = value % (int)m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}

void handle_resize() {
    window.screen_width = GetScreenWidth();
    window.screen_height = GetScreenHeight();
    window.scale = MIN(
        window.screen_width / WINDOW_WIDTH,
        window.screen_height / WINDOW_HEIGHT
    );
    window.offset_x = (window.screen_width - window.scale * WINDOW_WIDTH) / 2;
    window.offset_y = (window.screen_height - window.scale * WINDOW_HEIGHT) / 2;
}
