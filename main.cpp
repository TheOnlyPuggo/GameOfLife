#include <iostream>
#include <raylib.h>
#include <cmath>
#include <vector>

#include "raymath.h"
#include "Headers/RaylibOverloads.h"
#include "Headers/Util.h"
#include "Headers/Components.h"

int Cell::cellSize = 25;

std::vector<Cell*> cells{};

// CAMERA
Camera2D camera{};
Vector2 cameraPos{};
constexpr float mouseWheelMax{ 5.0f };
constexpr float mouseWheelMin{ 0.3f };
float currentMouseZoom{ 1.0f };

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

    // CAMERA PROPS
    camera.target = { screenWidth/2.0, screenHeight/2.0f};
    camera.offset = { screenWidth/2.0, screenHeight/2.0f};
    camera.rotation = 0.0f;

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
    placingCell.Update(cells, camera);

    if (std::size(cells) != 0)
    {
        for (Cell* cell : cells)
        {
            cell->Update(cells, camera);
        }
    }

    // CAMERA
    // MOVEMENT
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    cameraPos = camera.target - camera.offset;

    // ZOOMING
    currentMouseZoom += GetMouseWheelMove() * ((sin(currentMouseZoom)/2 + 0.5f)/mouseWheelMax);
    if (currentMouseZoom >= mouseWheelMax)
        currentMouseZoom = mouseWheelMax;
    if (currentMouseZoom <= mouseWheelMin)
        currentMouseZoom = mouseWheelMin;

    camera.zoom = currentMouseZoom;

    // GRID
    gridLineHorizontal.currentMouseZoom = currentMouseZoom;
    gridLineVertical.currentMouseZoom = currentMouseZoom;
}

void DrawGame()
{
    BeginDrawing();

        BeginMode2D(camera);
    
        ClearBackground(RAYWHITE);
        gridLineVertical.Draw(cameraPos);
        gridLineHorizontal.Draw(cameraPos);

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




