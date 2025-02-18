#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <cmath>
#include <vector>

#include "raymath.h"
#include "Headers/GameCamera.h"
#include "Headers/Util.h"
#include "Headers/RaylibOverloads.h"

#include "Headers/Components.h"

int Cell::cellSize = 25;
bool Cell::placingActive = true;

std::vector<Cell*> cells{};

constexpr int screenWidth { 1000 };
constexpr int screenHeight { 700 };
constexpr int targetFPS{ 200 };

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UnloadGame();
static void UpdateDrawFrame();

constexpr float cellSize = 25.0f;

// Components Init

GameCamera gameCamera{ 2.0f, 0.3f, 1.0f };

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

Cell placingCell{true, Color{255, 94, 94, 100}, 0.0f};

std::vector<Button*> buttons{};

Button* clearButton = new Button{
    "../Assets/bin-icon.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2
};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Game of Life");

    InitGame();

    //SetTargetFPS(targetFPS);

    while (!WindowShouldClose())
        UpdateDrawFrame();

    UnloadGame();

    CloseWindow();
    return 0;
}

void InitGame()
{
    gameCamera.Init();
    clearButton->Init();

    buttons.push_back(clearButton);
}

void UpdateGame()
{
    placingCell.Update(cells, gameCamera.m_camera);

    if (std::size(cells) != 0)
    {
        for (Cell* cell : cells)
        {
            if (cell->markedForDeletion == false)
                cell->Update(cells, gameCamera.m_camera);
        }

        cells.erase(std::remove_if(cells.begin(), cells.end(), [](Cell* cell)
        {
            if (cell->markedForDeletion)
            {
                delete cell;
                cell = nullptr;
                return true;
            }
            return false;
        }), cells.end());
    }

    // CAMERA
    gameCamera.Update();

    // BUTTON STUFF
    clearButton->Update(cells);

    int i{0};
    const float margin{40.0f};
    const float distanceFromBottom{80.0f};
    const float totalButtonsLength{(buttons[0]->GetWidth() + margin) * (float)std::size(buttons)};
    for (Button* button : buttons)
    {
        button->m_btnPos = Vector2(
            ((float)GetScreenWidth()/2.0f + (button->GetWidth() + margin) * i) - totalButtonsLength/2.0f + margin/2.0f,
            (float)GetScreenHeight() - distanceFromBottom
        );

        ++i;
    }

    //std::cout << i << '\n';

}

void DrawGame()
{
    BeginDrawing();

    BeginMode2D(gameCamera.m_camera);

    ClearBackground(RAYWHITE);
    gridLineVertical.Draw(gameCamera);
    gridLineHorizontal.Draw(gameCamera);

    placingCell.Draw(gameCamera);

    if (std::size(cells) != 0)
    {
        for (const Cell* cell : cells)
        {
            cell->Draw(gameCamera);
        }
    }

    EndMode2D();

    clearButton->Draw();

    EndDrawing();
}

void UnloadGame()
{
    for (const Cell* cell : cells)
    {
        delete cell;
        cell = nullptr;
    }
    cells.clear();

    for (const Button* button : buttons)
    {
        delete button;
        button = nullptr;
    }
    buttons.clear();
}

void UpdateDrawFrame()
{
    UpdateGame();
    DrawGame();
}
