//
// Created by iakin on 17/02/2025.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string_view>
#include <array>
#include <vector>
#include <raylib.h>

#include "GameCamera.h"

class Cell
{
private:
    bool m_beingPlaced{};
    Color m_color{};
    float m_despawnDistance{};

    int m_savedStep{};

public:
    static int cellSize;
    static bool placingActive;
    static std::vector<GridPos> cellsToBeSpawned;

    static bool readyForCellSpawn;
    bool m_thisCellReadyForSpawn{false};

    static std::array<GridPos, 8> cellNeighbourPositions;

    mutable bool m_markedForDeletion{false};

    GridPos m_gridPos{};

    Cell(const bool beingPlaced, const Color color, const float despawnDistance, const int savedStep)
    {
        m_beingPlaced = beingPlaced;
        m_color = color;
        m_despawnDistance = despawnDistance;
        m_savedStep = savedStep;
    }

    void Update(std::vector<Cell*>& cells, const Camera2D &camera, const int& currentStep, const bool& gamePaused)
    {
        // Handles the cell that moves with your cursor, 'placingActive' is for when your hovering over a button
        if (m_beingPlaced && placingActive && gamePaused)
        {
            m_gridPos = GridConversions::GetWorldToGridSpace((GetScreenToWorld2D(GetMousePosition(), camera)), cellSize);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                for (const Cell* cell : cells)
                {
                    if (cell->m_gridPos == m_gridPos)
                    {
                        return;
                    }
                }

                auto newCell = new Cell{false, BLUE, 1500.0f, currentStep}; // Memory should be deallocated in all possible outcomes... hopefully. So ignoring compiler warning.
                newCell->m_gridPos = m_gridPos;
                cells.push_back(newCell);
                newCell = nullptr;
            }

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                int i{0};
                for (const Cell* cell : cells)
                {
                    if (cell->m_gridPos == m_gridPos)
                    {
                        const Cell *cellTrack = cell;
                        cells.erase(cells.begin() + i);
                        delete cellTrack;
                        cellTrack = nullptr;
                    }
                    ++i;
                }
            }
        }

        // Runs when step is increased, handles decisions of self-destruction and new cell spawns.
        if (currentStep > m_savedStep && !m_beingPlaced)
        {
            m_savedStep = currentStep;

            int neighbourCount{0};

            std::vector<GridPos> potentialNewCellPositions{};

            for (GridPos neighbourGridPos : cellNeighbourPositions)
            {
                bool cellFound{false};
                for (const Cell* cell : cells)
                {
                    if (cell->m_gridPos == m_gridPos)
                        continue;

                    if (m_gridPos + neighbourGridPos == cell->m_gridPos)
                    {
                        ++neighbourCount;
                        cellFound = true;
                        break;
                    }
                }

                if (!cellFound)
                {
                    potentialNewCellPositions.push_back(m_gridPos + neighbourGridPos);
                }
            }

            if (!(neighbourCount == 2 || neighbourCount == 3))
                m_markedForDeletion = true;

            for (const GridPos potentialCellPos : potentialNewCellPositions)
            {
                int potentialNeighbourCount{0};

                for (GridPos neighbourGridPos : cellNeighbourPositions)
                {
                    for (const Cell* cell : cells)
                    {
                        if (potentialCellPos + neighbourGridPos == cell->m_gridPos)
                        {
                            ++potentialNeighbourCount;
                        }

                        if (potentialNeighbourCount > 3)
                            break;
                    }

                    if (potentialNeighbourCount > 3)
                        break;
                }

                if (potentialNeighbourCount == 3)
                {
                    if (std::find(cellsToBeSpawned.begin(), cellsToBeSpawned.end(), potentialCellPos) == cellsToBeSpawned.end())
                        cellsToBeSpawned.push_back(potentialCellPos);
                }
            }

            m_thisCellReadyForSpawn = true;
        }

        // This section makes sure to separate the previous section from the spawn new cells section, as otherwise they will interfere with the previous sections' outcomes.
        readyForCellSpawn = true;
        for (const Cell* cell : cells)
        {
            if (cell->m_thisCellReadyForSpawn == false)
                readyForCellSpawn = false;
        }

        if (cells.size() == 0)
        {
            readyForCellSpawn = false;
        }

        if (readyForCellSpawn)
        {
            SpawnCells(currentStep, cells);
        }
    }

    void Draw(const GameCamera& gameCamera, bool& gamePaused) const
    {
        if (m_beingPlaced && !gamePaused)
            return;

        Vector2 cameraToGridPos{ (gameCamera.m_cameraPos + gameCamera.m_camera.offset) - GridConversions::GetGridToWorldSpace(m_gridPos, cellSize) };
        float despawnDistanceMod{ m_despawnDistance * 1/gameCamera.GetCurrentMouseZoom() };

        if ( // Checks if both x or y of cameraToGrisPos is greater than despawnDistanceMod and m_beingPlaces is not true
            !(
                ((cameraToGridPos.x > despawnDistanceMod || cameraToGridPos.x < -despawnDistanceMod)
                || (cameraToGridPos.y > despawnDistanceMod || cameraToGridPos.y < -despawnDistanceMod))
                && !m_beingPlaced)
            )
        {
            DrawRectangle(
                (int)GridConversions::GetGridToWorldSpace(m_gridPos, cellSize).x,
                (int)GridConversions::GetGridToWorldSpace(m_gridPos, cellSize).y,
                cellSize,
                cellSize,
                m_color
            );
        }
    }

    static void SpawnCells(const int& currentStep, std::vector<Cell*>& cells)
    {
        if (!cellsToBeSpawned.empty() && readyForCellSpawn)
        {
            for (GridPos newCellPos : cellsToBeSpawned)
            {
                auto newCell = new Cell{false, BLUE, 1500.0f, currentStep};
                newCell->m_gridPos = newCellPos;
                cells.push_back(newCell);
                newCell = nullptr;
            }
        }

        cellsToBeSpawned.clear();
        for (Cell* cell : cells)
        {
            cell->m_thisCellReadyForSpawn = false;
        }

        readyForCellSpawn = false;
    }
};

class GridLine
{
private:
    int m_gridLineAmount{};
    float m_thickness{};
    Color m_color{};
    bool m_isHorizontal{};

public:
    GridLine(
        const int gridLineAmount,
        const float thickness,
        const Color color,
        const bool isHorizontal
        )
    {
        m_gridLineAmount = gridLineAmount;
        m_thickness = thickness;
        m_color = color;
        m_isHorizontal = isHorizontal;
    }

    void Draw(const GameCamera& game_camera) const
    {
        // Draws and moves the grid relative to camera, but at intervals of cellSize on both the x and y-axis to give the illusion the grid isn't moving.
        constexpr float gridScreenOffset{ 2500.0f };
        const float spawnGridAmount = (float)m_gridLineAmount * (1/game_camera.GetCurrentMouseZoom());

        for (int i{0}; i <= (int)spawnGridAmount-1; ++i)
        {
            if (m_isHorizontal)
            {
                DrawLineEx(
                    GridConversions::GetWorldGridNormalized(
                        Vector2((0.0f + game_camera.m_cameraPos.x) - gridScreenOffset,
                        (float)Cell::cellSize * (float)i + game_camera.m_cameraPos.y - gridScreenOffset),
                        Cell::cellSize
                    ),
                    GridConversions::GetWorldGridNormalized(
                        Vector2((float)GetScreenWidth() * (float)m_gridLineAmount + game_camera.m_cameraPos.x,
                        (float)Cell::cellSize * (float)i + game_camera.m_cameraPos.y - gridScreenOffset),
                        Cell::cellSize
                    ),
                    m_thickness,
                    m_color
                );
            } else
            {
                DrawLineEx(
                     GridConversions::GetWorldGridNormalized(
                         Vector2((float)Cell::cellSize * (float)i + game_camera.m_cameraPos.x - gridScreenOffset,
                         0.0f + game_camera.m_cameraPos.y - gridScreenOffset),
                         Cell::cellSize
                     ),
                     GridConversions::GetWorldGridNormalized(
                         Vector2((float)Cell::cellSize * (float)i + game_camera.m_cameraPos.x - gridScreenOffset,
                         (float)GetScreenHeight() * (float)m_gridLineAmount + game_camera.m_cameraPos.y),
                         Cell::cellSize
                     ),
                     m_thickness,
                     m_color
                 );
            }
        }
    }
};

class Button
{
private:
    std::string_view m_texturePath{};
    Rectangle m_sourceRec{};
    int m_scaleWidthMulti{};
    int m_scaleHeightMulti{};

    Rectangle m_btnBounds{};
    Texture2D m_texture{};
    bool m_btnHovered{false};

public:
    enum ButtonFunction
    {
        CLEAR_CELLS, STEP_BACK, STEP_FORWARD, PLAY, PAUSE
    };

    Vector2 m_btnPos{};
    ButtonFunction m_buttonFunction{};

    Button(std::string_view texturePath, Rectangle sourceRec, int scaleWidthMulti, int scaleHeightMulti, ButtonFunction buttonFunction)
    {
        m_texturePath = texturePath;
        m_sourceRec = sourceRec;
        m_scaleWidthMulti = scaleWidthMulti;
        m_scaleHeightMulti = scaleHeightMulti;
        m_buttonFunction = buttonFunction;
    }

    void Init()
    {
        m_texture = LoadTexture(m_texturePath.data());
        m_texture.height *= m_scaleWidthMulti;
        m_texture.width *= m_scaleHeightMulti;
    }

    void Update(
        std::vector<Cell*>& cells,
        int& currentStep,
        std::vector<std::vector<Cell*>>& savedCells,
        std::vector<Button*>& buttons,
        Button*& playButton,
        Button*& pauseButton,
        bool& gamePaused
    )
    {
        m_btnBounds = Rectangle{ m_btnPos.x, m_btnPos.y, m_sourceRec.width, m_sourceRec.height };

        if (CheckCollisionPointRec(GetMousePosition(), m_btnBounds))
        {
            m_btnHovered = true;
            Cell::placingActive = false;
        }
        else
        {
            if (m_btnHovered)
            {
                Cell::placingActive = true;
                m_btnHovered = false;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_btnHovered)
        {
            if (m_buttonFunction == CLEAR_CELLS)
                RemoveAllCells(currentStep, cells, savedCells);
            else if (m_buttonFunction == STEP_BACK)
                DecrementStep(currentStep, cells, savedCells);
            else if (m_buttonFunction == STEP_FORWARD)
                IncrementStep(currentStep, cells, savedCells);
            else if (m_buttonFunction == PLAY)
                PlaySteps(buttons, pauseButton, gamePaused);
            else if (m_buttonFunction == PAUSE)
                PauseSteps(buttons, playButton, gamePaused);
        }
    }

    void Draw()
    {
        if (!m_btnHovered)
            DrawTextureRec(m_texture, m_sourceRec, {m_btnBounds.x, m_btnBounds.y}, WHITE);
        else
            DrawTextureRec(m_texture, m_sourceRec, {m_btnBounds.x, m_btnBounds.y}, Color(255/2,255/2,255/2,100));
    }

    float GetWidth() const
    {
        return m_sourceRec.width;
    }

    float GetHeight() const
    {
        return m_sourceRec.height;
    }

    // BUTTON FUNCTIONS
    static void RemoveAllCells(int& currentStep, const std::vector<Cell*>& cells, std::vector<std::vector<Cell*>>& savedCells)
    {
        for (const Cell* cell : cells)
        {
            cell->m_markedForDeletion = true;
            savedCells.clear();
            currentStep = 0;
        }
    }

    static void IncrementStep(int& currentStep, std::vector<Cell*> cells, std::vector<std::vector<Cell*>>& savedCells)
    {
        if (cells.size() == 0)
            return;

        ++currentStep;

        // Creates and saves the current cell map to savesCells, which is what decrementStep will use to go back in steps.
        std::vector<Cell*> newCellMap{};
        for (Cell* cell : cells)
        {
            newCellMap.push_back(new Cell(*cell));
        }

        savedCells.push_back(newCellMap);
    }

    static void DecrementStep(int& currentStep, std::vector<Cell*>& cells, std::vector<std::vector<Cell*>>& savedCells)
    {
        if (currentStep == 0)
            return;

        --currentStep;

        for (Cell* cell : cells)
        {
            delete cell;
            cell = nullptr;
        }
        cells.clear();

        if (savedCells.size() == 0)
        {
            savedCells.clear();
        } else
        {
            for (Cell* cell : savedCells[currentStep])
            {
                cells.push_back(cell);
            }

            savedCells.pop_back();
        }
    }

    static void PlaySteps(std::vector<Button*>& buttons, Button*& pauseButton, bool& gamePaused)
    {
        buttons.erase(buttons.begin() + 2);
        buttons.insert(buttons.begin() + 2, pauseButton);
        pauseButton->Init();
        gamePaused = false;
    }

    static void PauseSteps(std::vector<Button*>& buttons, Button*& playButton, bool& gamePaused)
    {
        buttons.erase(buttons.begin() + 2);
        buttons.insert(buttons.begin() + 2, playButton);
        playButton->Init();
        gamePaused = true;
    }
};

#endif //COMPONENTS_H
