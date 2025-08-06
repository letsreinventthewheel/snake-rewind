#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdlib.h>
#include <time.h>

#include "stb_ds.h"

#define SCORE_ANIMATION_DURATION 0.3

#define STEP_INTERVAL 0.1
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

Font arcadeFont;

typedef struct {
    float scale;
    float angle;
    float duration;
} ScoreEffect;

typedef struct {
    float speed;
    float scale;
    float target_scale;
} ScaleEffect;

typedef struct {
    float duration;
    float intensity;
} ShakeEffect;

typedef enum {
    EMPTY_TILE,
    VISITED_TILE,
    PLAYER_TILE,
    FOOD_TILE,
    CLONE_TILE,
    CLONE_AND_PLAYER_TILE
} TileState;

typedef struct {
    float timer;
    float angle;
    TileState state;
    bool visited;
} Tile;

typedef struct {
    int row;
    int column;
} Position;

typedef enum {
    UP_DIRECTION,
    DOWN_DIRECTION,
    LEFT_DIRECTION,
    RIGHT_DIRECTION
} Direction;

typedef struct {
    Position *tiles;
    TileState value;
    Direction dir;
    Direction next_dir;
    Direction next_next_dir;
    bool has_next_next_dir;
} Snake;

typedef struct {
    Snake snake;
    size_t player_path_idx;
} SnakeClone;

typedef struct {
    Position position;
    TileState value;
} Food;

typedef struct {
    bool game_over;
    Tile tileGrid[ROWS][COLUMNS];
    Snake player;
    Food food;
    Position *player_path;
    SnakeClone *clones;
} Game;

Game game;

void InitTileGrid(void) {
    for (size_t row = 0; row < ROWS; row++) {
        for (size_t column = 0; column < COLUMNS; column++) {
            Tile *tile = &game.tileGrid[row][column];
            tile->state = EMPTY_TILE;
            tile->angle = 0;
            tile->timer = (row + 1) * (TILE_SIZE + TILE_SPACING) * (column + 1) * (TILE_SIZE + TILE_SPACING);
            tile->visited = false;
        }
    }
}

void InitSnake(Snake *snake, TileState value, size_t row, size_t column, size_t length, bool is_player) {
    snake->tiles = nullptr;
    snake->value = value;
    snake->dir = RIGHT_DIRECTION;
    snake->next_dir = RIGHT_DIRECTION;
    snake->next_next_dir = RIGHT_DIRECTION;
    snake->has_next_next_dir = false;

    Position start_position = (Position) {
        .row = row,
        .column = column
    };

    for (size_t i = 0; i < length; i++) {
        arrpush(snake->tiles, start_position);
    }

    if (is_player) {
        arrpush(game.player_path, start_position);
    }
}

void PlaceFoodRandomly(Food *food) {
    size_t row, column;
    do {
        row = rand() % ROWS;
        column = rand() % COLUMNS;
    } while (game.tileGrid[row][column].state != EMPTY_TILE && game.tileGrid[row][column].state != VISITED_TILE);

    food->position.row = row;
    food->position.column = column;
}

void InitFood(Food *food) {
    food->value = FOOD_TILE;
    PlaceFoodRandomly(food);
}

void InitGame(void) {
    game.player_path = nullptr;
    game.clones = nullptr;
    game.game_over = false;

    InitTileGrid();
    InitSnake(&game.player, PLAYER_TILE, 13, 24, 3, true);
    InitFood(&game.food);
}

Color GetTileColor(TileState state) {
    switch (state) {
        case EMPTY_TILE:
            return (Color) { 20, 20, 20, 255 };
        case VISITED_TILE:
            return (Color) { 50, 50, 50, 255 };
        case PLAYER_TILE:
            return (Color) { 255, 255, 255, 255 };
        case FOOD_TILE:
            return (Color) { 40, 255, 40, 255 };
        case CLONE_TILE:
            return (Color) { 255, 40, 40, 255 };
        case CLONE_AND_PLAYER_TILE:
            return (Color) { 200, 20, 160, 255 };
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
            if (game.food.position.row - row == 0 || game.food.position.column - column == 0) {
                color.r = Clamp(color.r + 10, 0, 255);
                color.g = Clamp(color.g + 10, 0, 255);
                color.b = Clamp(color.b + 10, 0, 255);
            }
            color = Fade(color, game.game_over ? 0.7 : 1.0);

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
            tile->state = tile->visited ? VISITED_TILE : EMPTY_TILE;
        }
    }
}

void SnakeMarkTiles(Snake *snake) {
    size_t len = arrlen(snake->tiles);
    for (size_t i = 0; i < len; i++) {
        Position *p = &snake->tiles[i];
        Tile *tile = &game.tileGrid[p->row][p->column];
        tile->visited = true;

        bool is_player_tile = tile->state == PLAYER_TILE || tile->state == CLONE_AND_PLAYER_TILE;
        bool is_clone_tile = tile->state == CLONE_TILE || tile->state == CLONE_AND_PLAYER_TILE;

        if ((is_player_tile && snake->value == CLONE_TILE) || (is_clone_tile && snake->value == PLAYER_TILE)) {
            tile->state = CLONE_AND_PLAYER_TILE;
        } else {
            tile->state = snake->value;
        }
    }
}

void SnakeDoStep(Snake *snake) {
    snake->dir = snake->next_dir;

    int dx = 0, dy = 0;
    if (snake->dir == UP_DIRECTION) dy = -1;
    else if (snake->dir == DOWN_DIRECTION) dy = 1;
    else if (snake->dir == LEFT_DIRECTION) dx = -1;
    else if (snake->dir == RIGHT_DIRECTION) dx = 1;

    Position *head = &snake->tiles[0];
    Position new_head = (Position) {
        .row = head->row + dy,
        .column = head->column + dx
    };

    if (new_head.row < 0) {
        new_head.row = ROWS - 1;
    } else if (new_head.row >= ROWS) {
        new_head.row = 0;
    } else if (new_head.column < 0) {
        new_head.column = COLUMNS - 1;
    } else if (new_head.column >= COLUMNS) {
        new_head.column = 0;
    }

    size_t len = arrlen(snake->tiles);
    for (size_t i = len - 1; i > 0; i--) {
        snake->tiles[i] = snake->tiles[i-1];
    }
    snake->tiles[0] = new_head;
    arrpush(game.player_path, new_head);

    if (snake->has_next_next_dir) {
        snake->next_dir = snake->next_next_dir;
        snake->has_next_next_dir = false;
    }
}

void SnakeHandleInput(Snake *snake) {
    if (snake->dir == snake->next_dir) {
        if (IsKeyPressed(KEY_LEFT) && snake->dir != RIGHT_DIRECTION) {
            snake->next_dir = LEFT_DIRECTION;
        }
        if (IsKeyPressed(KEY_RIGHT) && snake->dir != LEFT_DIRECTION) {
            snake->next_dir = RIGHT_DIRECTION;
        }
        if (IsKeyPressed(KEY_UP) && snake->dir != DOWN_DIRECTION) {
            snake->next_dir = UP_DIRECTION;
        }
        if (IsKeyPressed(KEY_DOWN) && snake->dir != UP_DIRECTION) {
            snake->next_dir = DOWN_DIRECTION;
        }
    } else {
        if (IsKeyPressed(KEY_LEFT) && snake->next_dir != RIGHT_DIRECTION) {
            snake->has_next_next_dir = true;
            snake->next_next_dir = LEFT_DIRECTION;
        }
        if (IsKeyPressed(KEY_RIGHT) && snake->next_dir != LEFT_DIRECTION) {
            snake->has_next_next_dir = true;
            snake->next_next_dir = RIGHT_DIRECTION;
        }
        if (IsKeyPressed(KEY_UP) && snake->next_dir != DOWN_DIRECTION) {
            snake->has_next_next_dir = true;
            snake->next_next_dir = UP_DIRECTION;
        }
        if (IsKeyPressed(KEY_DOWN) && snake->next_dir != UP_DIRECTION) {
            snake->has_next_next_dir = true;
            snake->next_next_dir = DOWN_DIRECTION;
        }
    }
}

void FoodMarkTile(Food *food) {
    Tile *tile = &game.tileGrid[food->position.row][food->position.column];
    tile->state = food->value;
}

void SnakeGrow(Snake *snake) {
    size_t len = arrlen(snake->tiles);
    Position last = snake->tiles[len-1];
    arrpush(snake->tiles, last);
}

void SpawnClone(Snake *player) {
    Snake snake;
    size_t row = game.player_path[0].row;
    size_t column = game.player_path[0].column;
    size_t length = arrlen(player->tiles);
    InitSnake(&snake, CLONE_TILE, row, column, length, false);

    SnakeClone clone = (SnakeClone) {
        .snake = snake,
        .player_path_idx = 0
    };

    arrpush(game.clones, clone);
}

void ClonesMarkTiles() {
    size_t clones_len = arrlen(game.clones);
    for (size_t i = 0; i < clones_len; i++) {
        SnakeMarkTiles(&game.clones[i].snake);
    }
}

void MoveClones() {
    size_t clones_len = arrlen(game.clones);
    size_t player_path_len = arrlen(game.player_path);
    for (size_t i = 0; i < clones_len; i++) {
        SnakeClone *clone = &game.clones[i];

        if (clone->player_path_idx >= player_path_len) {
            continue;
        }

        Position next = game.player_path[clone->player_path_idx];
        size_t len = arrlen(clone->snake.tiles);
        for (size_t j = len - 1; j > 0; j--) {
            clone->snake.tiles[j] = clone->snake.tiles[j-1];
        }
        clone->snake.tiles[0] = next;
        clone->player_path_idx++;
    }
}

void ReduceClones(void) {
    size_t clones_len = arrlen(game.clones);
    for (int i = clones_len - 1; i >= 0; i--) {
        SnakeClone *clone = &game.clones[i];

        arrpop(clone->snake.tiles);

        if (arrlen(clone->snake.tiles) == 0) {
            arrdelswap(game.clones, i);
        }
    }
}

bool CheckForCollisions(Snake *player) {
    Position *head = &player->tiles[0];
    size_t len = arrlen(player->tiles);
    for (size_t i = 1; i < len; i++) {
        Position *p = &player->tiles[i];
        if (p->row == head->row && p->column == head->column) {
            return true;
        }
    }

    size_t clones_len = arrlen(game.clones);
    for (size_t i = 0; i < clones_len; i++) {
        SnakeClone *clone = &game.clones[i];
        len = arrlen(clone->snake.tiles);
        for (size_t j = 0; j < len; j++) {
            Position *p = &clone->snake.tiles[j];
            if (p->row == head->row && p->column == head->column) {
                return true;
            }
        }
    }

    return false;
}

void RestartGame(void) {
    if (game.player_path) {
        arrfree(game.player_path);
    }
    if (game.clones) {
        arrfree(game.clones);
    }
    InitGame();
}

void DrawGameOver(void) {
    const char *game_over_text = "GAME OVER";
    size_t game_over_font_size = 70;
    Vector2 game_over_size = MeasureTextEx(arcadeFont, game_over_text, game_over_font_size, 0);
    DrawTextEx(
        arcadeFont,
        game_over_text,
        (Vector2) {
            .x = (GAME_WIDTH - game_over_size.x) / 2.0,
            .y = (GAME_HEIGHT - game_over_size.y) / 3.0
        },
        game_over_font_size,
        0,
        WHITE
    );

    const char *restart_game_text = "PRESS ENTER TO RESTART";
    size_t restart_game_font_size = 24;
    Vector2 restart_game_size = MeasureTextEx(arcadeFont, restart_game_text, restart_game_font_size, 0);
    DrawTextEx(
        arcadeFont,
        restart_game_text,
        (Vector2) {
            .x = (GAME_WIDTH - restart_game_size.x) / 2.0,
            .y = (GAME_HEIGHT - restart_game_size.y) * 2 / 3.0
        },
        restart_game_font_size,
        0,
        WHITE
    );
}

void UpdateScaleEffect(ScaleEffect *effect, float dt) {
    if (effect->scale != effect->target_scale) {
        effect->scale -= (effect->scale - effect->target_scale) * dt * effect->speed;
        if (fabs(effect->scale - effect->target_scale) < 0.001) {
            effect->scale = effect->target_scale;
        }
    }
}

void UpdateShakeEffect(ShakeEffect *effect, float dt) {
    if (effect->duration > 0) {
        effect->duration -= dt;
        if (effect->duration < 0) {
            effect->duration = 0;
        }
    }
}

void DrawScore(ScoreEffect *effect) {
    char score_text[32];
    size_t score_text_font_size = 32;
    size_t score = arrlen(game.player.tiles);
    snprintf(score_text, sizeof(score_text), "LENGTH: %zu", score);

    Vector2 score_text_size = MeasureTextEx(arcadeFont, score_text, score_text_font_size, 0);

    Vector2 draw_position = (Vector2) {
        .x = GAME_WIDTH / 2.0 - score_text_size.x / 2.0,
        .y = 4
    };

    Vector2 new_origin = (Vector2) {
        .x = GAME_WIDTH / 2.0,
        .y = 4 + score_text_size.y / 2.0
    };

    rlPushMatrix();
    rlTranslatef(new_origin.x, new_origin.y, 0);
    rlRotatef(effect->angle, 0, 0, 1);
    rlScalef(effect->scale, effect->scale, 1);
    rlTranslatef(-new_origin.x, -new_origin.y, 0);
    DrawTextEx(arcadeFont, score_text, draw_position, score_text_font_size, 0, WHITE);
    rlPopMatrix();
}

void UpdateScoreEffect(ScoreEffect *effect, float dt) {
    if (effect->duration > 0) {
        effect->duration -= dt;

        if (effect->duration <= 0) {
            effect->duration = 0;
            effect->scale = 1;
            effect->angle = 0;
        } else {
            float t = 1.0 - effect->duration / SCORE_ANIMATION_DURATION;
            effect->scale = Lerp(1.3, 1.0, t);
            effect->angle = Lerp(effect->angle, 0, t);
        }
    }
}

int main(void) {
    srand(time(nullptr));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Rewind");

    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    arcadeFont = LoadFont("assets/fonts/ARCADE_N.TTF");

    ScaleEffect scale_effect = (ScaleEffect) {
        .scale = 1.0,
        .target_scale = 1.0,
        .speed = 5.0
    };

    ShakeEffect shake_effect = (ShakeEffect) {
        .duration = 0,
        .intensity = 20
    };

    ScoreEffect score_effect = (ScoreEffect) {
        .duration = 0,
        .angle = 0,
        .scale = 1.0
    };

    InitGame();

    bool foodWasEaten = false;
    float stepTimer = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        stepTimer += dt;

        SnakeHandleInput(&game.player);

        UpdateTileGrid(dt);
        UpdateScaleEffect(&scale_effect, dt);
        UpdateShakeEffect(&shake_effect, dt);
        UpdateScoreEffect(&score_effect, dt);

        if (stepTimer >= STEP_INTERVAL) {
            MoveClones();

            if (!game.game_over) {
                SnakeDoStep(&game.player);

                Position *head = &game.player.tiles[0];
                if (head->row == game.food.position.row && head->column == game.food.position.column) {
                    ReduceClones();
                    SpawnClone(&game.player);
                    SnakeGrow(&game.player);
                    foodWasEaten = true;
                    score_effect.duration = SCORE_ANIMATION_DURATION;
                    score_effect.angle = GetRandomValue(-10, 10);
                    score_effect.scale = 1.3;
                }

                game.game_over = CheckForCollisions(&game.player);
                if (game.game_over) {
                    scale_effect.scale = 1.3;
                    shake_effect.duration = 0.3;
                }
            }

            stepTimer = 0;
        }

        SnakeMarkTiles(&game.player);
        ClonesMarkTiles();
        if (foodWasEaten) {
            foodWasEaten = false;
            PlaceFoodRandomly(&game.food);
        }
        FoodMarkTile(&game.food);

        BeginTextureMode(target);
            ClearBackground(BLACK);
            DrawTileGrid();
            DrawScore(&score_effect);
            if (game.game_over) {
                DrawGameOver();
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            float scaledWidth = WINDOW_WIDTH * scale_effect.scale;
            float scaledHeight = WINDOW_HEIGHT * scale_effect.scale;

            Vector2 shake_offset = {0};
            if (shake_effect.duration > 0) {
                shake_offset.x = GetRandomValue(-shake_effect.intensity, shake_effect.intensity);
                shake_offset.y = GetRandomValue(-shake_effect.intensity, shake_effect.intensity);
            }

            Rectangle destination = (Rectangle) {
                .x = -(scaledWidth - WINDOW_WIDTH) / 2.0 + shake_offset.x,
                .y = -(scaledHeight - WINDOW_HEIGHT) / 2.0 + shake_offset.y,
                .width = scaledWidth,
                .height = scaledHeight
            };

            DrawTexturePro(
                target.texture,
                (Rectangle) {0, 0, GAME_WIDTH, -GAME_HEIGHT},
                destination,
                (Vector2) { 0 },
                0,
                WHITE
            );
        EndDrawing();

        if (game.game_over && IsKeyPressed(KEY_ENTER)) {
            RestartGame();
        }
    }

    CloseWindow();

    return 0;
}
