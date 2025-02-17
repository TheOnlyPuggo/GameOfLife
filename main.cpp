#include <iostream>
#include <raylib.h>
#include <cmath>
#include <vector>

#include "Headers/RaylibOverloads.h"
#include "Headers/Util.h"
#include "Headers/Components.h"

int Cell::cellSize = 25;

std::vector<Cell*> cells{};

constexpr int screenWidth { 800 };
constexpr int screenHeight { 450 };
constexpr int targetFPS{ 60 };

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UnloadGame();
static void UpdateDrawFrame();

constexpr float cellSize = 25.0f;

// Components Init
GridLine gridLineVertical{
    40,
    2.0f,
    LIGHTGRAY,
    false
};

GridLine gridLineHorizontal{
    40,
    2.0f,
    LIGHTGRAY,
    true
};

Cell placingCell{true, Color{255, 94, 94, 100}};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game of Life");

    Camera2D camera{0};
    camera.zoom = 10.0f;

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

}

void UpdateGame()
{
    // test
    placingCell.Update(cells);

    if (std::size(cells) != 0)
    {
        for (Cell* cell : cells)
        {
            cell->Update(cells);
        }
    }
}

void DrawGame()
{
    BeginDrawing();
    
        ClearBackground(RAYWHITE);
        gridLineVertical.Draw();
        gridLineHorizontal.Draw();

        placingCell.Draw();

    if (std::size(cells) != 0)
    {
        for (const Cell* cell : cells)
        {
            cell->Draw();
        }
    }

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




