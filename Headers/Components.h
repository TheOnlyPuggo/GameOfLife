//
// Created by iakin on 17/02/2025.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <raylib.h>

class Cell
{
private:
    bool m_beingPlaced{};
    Color m_color{};
    float m_despawnDistance{};

public:
    static int cellSize;

    GridPos m_gridPos{};

    Cell(const bool beingPlaced, const Color color, float despawnDistance)
    {
        m_beingPlaced = beingPlaced;
        m_color = color;
        m_despawnDistance = despawnDistance;
    }

    void Update(std::vector<Cell*>& cells, const Camera2D &camera)
    {
        if (m_beingPlaced)
        {
            m_gridPos = GridConversions::GetWorldToGridSpace(GetScreenToWorld2D(GetMousePosition(), camera), cellSize);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                for (const Cell* cell : cells)
                {
                    if (cell->m_gridPos == m_gridPos)
                    {
                        return;
                    }
                }

                auto newCell = new Cell{false, RED, 1500.0f}; // Memory should be deallocated in all possible outcomes... hopefully. So ignoring compiler warning.
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
    }

    void Draw(const GameCamera& gameCamera) const
    {
        Vector2 cameraToGridPos{ gameCamera.m_cameraPos - GridConversions::GetGridToWorldSpace(m_gridPos, cellSize) };
        float despawnDistanceMod{ m_despawnDistance * 1/gameCamera.GetCurrentMouseZoom() };

        if ( // Checks if both x and y of cameraToGrisPos is greater than despawnDistanceMod and m_beingPlaces is not true
            !(
                (cameraToGridPos.x > despawnDistanceMod || cameraToGridPos.x < -despawnDistanceMod)
                && (cameraToGridPos.y > despawnDistanceMod || cameraToGridPos.y < -despawnDistanceMod)
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
};

class GridLine
{
private:
    int m_gridLineAmount{};
    float m_thickness{};
    Color m_color{};
    bool m_isHorizontal{};

public:
    explicit GridLine(
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

#endif //COMPONENTS_H
