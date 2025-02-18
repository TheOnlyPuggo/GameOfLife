#include <iostream>
#include <raylib.h>
#include <cmath>
#include <vector>

#include "raymath.h"
#include "Headers/RaylibOverloads.h"
#include "Headers/GameCamera.h"
#include "Headers/Util.h"

#include "Headers/Components.h"

int Cell::cellSize = 25;

std::vector<Cell*> cells{};

constexpr int screenWidth { 1000 };
constexpr int screenHeight { 700 };
constexpr int targetFPS{ 60 };

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UnloadGame();
static void UpdateDrawFrame();

constexpr float cellSize = 25.0f;

// Components Init

GameCamera gameCamera{ 5.0f, 0.3f, 1.0f };

GridLine gridLineVertical{
    400,
    5.0f,
    LIGHTGRAY,
    false
};

GridLine gridLineHorizontal{
    400,
    5.0f,
    LIGHTGRAY,
    true
};

Cell placingCell{true, Color{255, 94, 94, 100}};

int main()
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game of Life");

    InitGame();

    SetTargetFPS(targetFPS);

    while (!WindowShouldClose())
        UpdateDrawFrame();

    UnloadGame();

    CloseWindow();
    return 0;
}

void InitGame()
{
    gameCamera.Init();
}

void UpdateGame()
{
    placingCell.Update(cells, gameCamera.m_camera);

    if (std::size(cells) != 0)
    {
        for (Cell* cell : cells)
        {
            cell->Update(cells, gameCamera.m_camera);
        }
    }

    // CAMERA
    gameCamera.Update();

    // MOVEMENT
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/gameCamera.m_camera.zoom);
        gameCamera.m_camera.target = Vector2Add(gameCamera.m_camera.target, delta);
    }

}

void DrawGame()
{
    BeginDrawing();

        BeginMode2D(gameCamera.m_camera);
    
        ClearBackground(RAYWHITE);
        gridLineVertical.Draw(gameCamera);
        gridLineHorizontal.Draw(gameCamera);

        placingCell.Draw();

        if (std::size(cells) != 0)
        {
            for (const Cell* cell : cells)
            {
                cell->Draw();
            }
        }

        EndMode2D();

    EndDrawing();
}

void UnloadGame()
{
    if (std::size(cells) > 0)
    {
        for (const Cell* cell : cells)
        {
            delete cell;
            cell = nullptr;
        }
    }
}

void UpdateDrawFrame()
{
    UpdateGame();
    DrawGame();
}




