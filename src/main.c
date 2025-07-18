#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <raylib.h>
#include <rlgl.h>

#define SCALE 0.75

#define BASE_WIDTH 1920
#define BASE_HEIGHT 1080

#define WINDOW_WIDTH ((int)(BASE_WIDTH * SCALE))
#define WINDOW_HEIGHT ((int)(BASE_HEIGHT * SCALE))

#define GAME_WIDTH ((int)(BASE_WIDTH / 2)) // 960
#define GAME_HEIGHT ((int)(BASE_HEIGHT / 2))

#define ROWS 28
#define COLUMNS 52
#define TILE_SIZE 16
#define TILE_SPACING 2

#define GRID_OFFSET_X 12
#define GRID_OFFSET_Y 35

typedef enum {
    EMPTY_TILE,
    VISITED_TILE,
} TileState;

typedef struct {
    float timer;
    float angle;
    TileState state;
} Tile;

typedef struct {
    Tile tileGrid[ROWS][COLUMNS];
} Game;

Game game;

void InitTileGrid(void) {
    for (size_t row = 0; row < ROWS; row++) {
        for (size_t column = 0; column < COLUMNS; column++) {
            Tile *tile = &game.tileGrid[row][column];
            tile->state = EMPTY_TILE;
            tile->angle = 0;
            tile->timer = (row + 1) * (TILE_SIZE + TILE_SPACING) * (column + 1) * (TILE_SIZE + TILE_SPACING);

            if (row > 5 && row < 10) {
                if ( column > 3 && column < 7) {
                    tile->state = VISITED_TILE;
                }
            }
        }
    }
}

void InitGame(void) {
    InitTileGrid();
}

Color GetTileColor(TileState state) {
    switch (state) {
        case EMPTY_TILE:
            return (Color) { 70, 70, 70, 255 };
        case VISITED_TILE:
            return (Color) { 50, 50, 50, 255 };
    }
}

void DrawTileGrid(void) {
    for (size_t row = 0; row < ROWS; row++) {
        for (size_t column = 0; column < COLUMNS; column++) {
            Tile *tile = &game.tileGrid[row][column];

            float drawX = column * (TILE_SIZE + TILE_SPACING) + GRID_OFFSET_X;
            float drawY = row * (TILE_SIZE + TILE_SPACING) + GRID_OFFSET_Y;

            Vector2 position = (Vector2) {
                .x = drawX + TILE_SPACING / 2.0 + (tile->state == EMPTY_TILE ? TILE_SIZE / 4.0 : 0),
                .y = drawY + TILE_SPACING / 2.0 + (tile->state == EMPTY_TILE ? TILE_SIZE / 4.0 : 0),
            };
            Vector2 size = (Vector2) {
                .x = tile->state == EMPTY_TILE ? TILE_SIZE / 2.0 : TILE_SIZE,
                .y = tile->state == EMPTY_TILE ? TILE_SIZE / 2.0 : TILE_SIZE,
            };
            Color color = GetTileColor(tile->state);

            Vector2 center = (Vector2) {
                .x = drawX + TILE_SPACING / 2.0 + TILE_SIZE / 2.0,
                .y = drawY + TILE_SPACING / 2.0 + TILE_SIZE / 2.0,
            };

            rlPushMatrix();
            rlTranslatef(center.x, center.y, 0);
            rlRotatef(tile->state == EMPTY_TILE ? (RAD2DEG * tile->angle) : 0, 0, 0, 1);
            rlTranslatef(-center.x, -center.y, 0);
            DrawRectangleV(position, size, color);
            rlPopMatrix();
        }
    }
}

void UpdateTileGrid(float dt) {
    for (size_t row = 0; row < ROWS; row++) {
        for (size_t column = 0; column < COLUMNS; column++) {
            Tile *tile = &game.tileGrid[row][column];
            tile->timer += dt;
            tile->angle = sinf(tile->timer) * PI;
        }
    }
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib [shapes] example - raylib logo using shapes");

    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);

    InitGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateTileGrid(dt);

        BeginTextureMode(target);
            ClearBackground(BLACK);
            DrawTileGrid();
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            DrawTexturePro(
                target.texture,
                (Rectangle) {0, 0, GAME_WIDTH, -GAME_HEIGHT},
                (Rectangle) {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
                (Vector2) { 0 },
                0,
                WHITE
            );
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
