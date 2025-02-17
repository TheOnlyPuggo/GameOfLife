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

public:
    static int cellSize;

    Vector2 m_pos{};

    Cell(const bool beingPlaced, const Color color)
    {
        m_beingPlaced = beingPlaced;
        m_color = color;
    }

    void Update(std::vector<Cell*>& cells, Camera2D camera)
    {
        if (m_beingPlaced)
        {
            m_pos = GridConversions::GetWorldGridNormalized(GetScreenToWorld2D(GetMousePosition(), camera), cellSize);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                for (const Cell* cell : cells)
                {
                    if (cell->m_pos == m_pos)
                    {
                        return;
                    }
                }

                auto newCell = new Cell{false, RED}; // Memory should be deallocated in all possible outcomes... hopefully. So ignoring compiler warning.
                newCell->m_pos = m_pos;
                cells.push_back(newCell);
                newCell = nullptr;
            }

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                int i{0};
                for (const Cell* cell : cells)
                {
                    if (cell->m_pos == m_pos)
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

    void Draw() const
    {
        DrawRectangle((int)m_pos.x, (int)m_pos.y, cellSize, cellSize, m_color);
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

    void Draw(Vector2 cameraPos) const
    {
        const float gridScreenOffset{ 1500.0f };

        for (int i{0}; i <= m_gridLineAmount-1; ++i)
        {
            if (m_isHorizontal)
            {
                DrawLineEx(
                    GridConversions::GetWorldGridNormalized(Vector2((0.0f + cameraPos.x) - gridScreenOffset, (float)Cell::cellSize * (float)i + cameraPos.y - gridScreenOffset), Cell::cellSize),
                    GridConversions::GetWorldGridNormalized(Vector2((float)GetScreenWidth() * (float)m_gridLineAmount + cameraPos.x, (float)Cell::cellSize * (float)i + cameraPos.y - gridScreenOffset), Cell::cellSize),
                    m_thickness,
                    m_color
                );
            } else
            {
                DrawLineEx(
                     GridConversions::GetWorldGridNormalized(Vector2((float)Cell::cellSize * (float)i + cameraPos.x - gridScreenOffset, 0.0f + cameraPos.y - gridScreenOffset), Cell::cellSize),
                     GridConversions::GetWorldGridNormalized(Vector2((float)Cell::cellSize * (float)i + cameraPos.x - gridScreenOffset, (float)GetScreenHeight() * (float)m_gridLineAmount + cameraPos.y), Cell::cellSize),
                     m_thickness,
                     m_color
                 );
            }
        }
    }
};

#endif //COMPONENTS_H
