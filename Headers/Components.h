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

    void Update(std::vector<Cell*>& cells)
    {
        if (m_beingPlaced)
        {
            m_pos = MouseProps::GetWorldGridNormalized(GetMousePosition(), cellSize);

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

    void Draw() const
    {
        for (int i{0}; i <= m_gridLineAmount-1; ++i)
        {
            if (m_isHorizontal)
            {
                DrawLineEx(
                    Vector2(0.0f, (float)Cell::cellSize * (float)i),
                    Vector2((float)GetScreenWidth(), (float)Cell::cellSize * (float)i),
                    m_thickness,
                    m_color
                );
            } else
            {
                DrawLineEx(
                     Vector2((float)Cell::cellSize * (float)i, 0.0f),
                     Vector2((float)Cell::cellSize * (float)i, (float)GetScreenHeight()),
                     m_thickness,
                     m_color
                 );
            }
        }
    }
};

#endif //COMPONENTS_H
