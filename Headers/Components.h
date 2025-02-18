//
// Created by iakin on 17/02/2025.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string_view>
#include <raylib.h>

#include "GameCamera.h"

class Cell
{
private:
    bool m_beingPlaced{};
    Color m_color{};
    float m_despawnDistance{};

public:
    static int cellSize;
    static bool placingActive;

    mutable bool markedForDeletion{false};

    GridPos m_gridPos{};

    Cell(const bool beingPlaced, const Color color, float despawnDistance)
    {
        m_beingPlaced = beingPlaced;
        m_color = color;
        m_despawnDistance = despawnDistance;
    }

    void Update(std::vector<Cell*>& cells, const Camera2D &camera)
    {
        if (m_beingPlaced && placingActive)
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
    Vector2 m_btnPos{};

    Button(std::string_view texturePath, Rectangle sourceRec, int scaleWidthMulti, int scaleHeightMulti)
    {
        m_texturePath = texturePath;
        m_sourceRec = sourceRec;
        m_scaleWidthMulti = scaleWidthMulti;
        m_scaleHeightMulti = scaleHeightMulti;
    }

    void Init()
    {
        m_texture = LoadTexture(m_texturePath.data());
        m_texture.height *= m_scaleWidthMulti;
        m_texture.width *= m_scaleHeightMulti;
    }

    void Update(std::vector<Cell*>& cells)
    {
        m_btnBounds = Rectangle{ m_btnPos.x, m_btnPos.y, m_sourceRec.width, m_sourceRec.height };

        if (CheckCollisionPointRec(GetMousePosition(), m_btnBounds))
        {
            m_btnHovered = true;
            Cell::placingActive = false;
        }
        else
        {
            m_btnHovered = false;
            Cell::placingActive = true;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_btnHovered)
        {
            RemoveAllCells(cells);
        }
    }

    void Draw()
    {
        if (!m_btnHovered)
            DrawTextureRec(m_texture, m_sourceRec, (Vector2){m_btnBounds.x, m_btnBounds.y}, WHITE);
        else
            DrawTextureRec(m_texture, m_sourceRec, (Vector2){m_btnBounds.x, m_btnBounds.y}, Color(255/2,255/2,255/2,100));
    }

    float GetWidth()
    {
        return m_sourceRec.width;
    }

    float GetHeight()
    {
        return m_sourceRec.height;
    }

    // BUTTON FUNCTIONS
    static void RemoveAllCells(const std::vector<Cell*>& cells)
    {
        for (const Cell* cell : cells)
        {
            cell->markedForDeletion = true;
        }
    }
};

#endif //COMPONENTS_H
