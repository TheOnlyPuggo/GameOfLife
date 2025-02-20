#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <cmath>
#include <vector>
#include <array>

#include "raymath.h"
#include "Headers/GameCamera.h"
#include "Headers/Util.h"
#include "Headers/RaylibOverloads.h"

#include "Headers/Components.h"

// Static Cell Var Initializations
int Cell::cellSize = 25;
bool Cell::placingActive = true;
std::vector<GridPos> Cell::cellsToBeSpawned{};
bool Cell::readyForCellSpawn = false;
std::array<GridPos, 8> Cell::cellNeighbourPositions{
    GridPos(0, -1),
    GridPos(1, -1),
    GridPos(1, 0),
    GridPos(1, 1),
    GridPos(0, 1),
    GridPos(-1, 1),
    GridPos(-1, 0),
    GridPos(-1, -1)
};

// Global Vars
int currentStep{0};

std::vector<Cell*> cells{};
std::vector<std::vector<Cell*>> savedCells{};

bool gamePaused{true};
double timeElapsed{};
int timeSpeed{10};

constexpr int screenWidth { 1000 };
constexpr int screenHeight { 700 };
constexpr int targetFPS{ 200 };

// Declaring primary functions ahead
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

Cell placingCell{true, Color{0, 0, 255, 50}, 0.0f, currentStep};

std::vector<Button*> buttons{};

Button* stepBackButton = new Button{
    "../Assets/step-back.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2,
    Button::STEP_BACK
};

Button* stepForwardButton = new Button{
    "../Assets/step-forward.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2,
    Button::STEP_FORWARD
};

Button* playButton = new Button{
    "../Assets/play-button.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2,
    Button::PLAY
};

Button* pauseButton = new Button{
    "../Assets/pause-button.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2,
    Button::PAUSE
};

Button* clearButton = new Button{
    "../Assets/bin-icon.png",
    Rectangle(0.0f, 0.0f, 64.0f, 64.0f),
    2,
    2,
    Button::CLEAR_CELLS
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
    buttons.push_back(stepBackButton);
    buttons.push_back(stepForwardButton);
    buttons.push_back(playButton);
    buttons.push_back(clearButton);

    for (Button* button : buttons)
    {
        button->Init();
    }
}

void UpdateGame()
{
    placingCell.Update(cells, gameCamera.m_camera, currentStep);

    if (std::size(cells) != 0)
    {
        for (Cell* cell : cells)
        {
            if (cell->m_markedForDeletion == false)
                cell->Update(cells, gameCamera.m_camera, currentStep);
        }

        // Delete any cells that have been marked for deletion.
        cells.erase(std::remove_if(cells.begin(), cells.end(), [](const Cell* cell)
        {
            if (cell->m_markedForDeletion)
            {
                delete cell;
                cell = nullptr;
                return true;
            }
            return false;
        }), cells.end());
    }

    gameCamera.Update();

    for (Button* button : buttons)
    {
        button->Update(
            cells,
            currentStep,
            savedCells,
            buttons,
            playButton,
            pauseButton,
            gamePaused
        );
    }

    // Lays out the buttons in the button array automatically
    int i{0};
    constexpr float margin{40.0f};
    constexpr float distanceFromBottom{80.0f};
    const float totalButtonsLength{(buttons[0]->GetWidth() + margin) * (float)std::size(buttons)};
    for (Button* button : buttons)
    {
        button->m_btnPos = Vector2(
            ((float)GetScreenWidth()/2.0f + (button->GetWidth() + margin) * (float)i) - totalButtonsLength/2.0f + margin/2.0f,
            (float)GetScreenHeight() - distanceFromBottom
        );

        ++i;
    }

    // Increment steps at a fixed player-decided interval is gamePaused is false
    if (gamePaused == true)
        timeElapsed = GetTime();

    if (gamePaused == false) {
        if (GetTime() - timeElapsed > (1/(double)timeSpeed))
        {
            timeElapsed = GetTime();
            Button::IncrementStep(currentStep, cells, savedCells);
        }
    }

    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL))
    {
        ++timeSpeed;
    }
    if ((IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS)) && timeSpeed != 0)
    {
        --timeSpeed;
    }
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

    for (Button* button : buttons)
    {
        button->Draw();
    }

    // Text Draws
    const std::string stepText{ "Step: " };
    const std::string speedText{ "Speed: " };
    DrawText((stepText + std::to_string(currentStep)).c_str(), 30, 30, 32, BLACK);
    DrawText((speedText + std::to_string(timeSpeed)).c_str(), 30, 60, 32, BLACK);

    EndDrawing();
}

void UnloadGame()
{
    // Deletes all cell and button objects on game end
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
