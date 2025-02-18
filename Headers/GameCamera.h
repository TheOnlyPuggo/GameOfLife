//
// Created by iakin on 18/02/2025.
//

#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include <raylib.h>

class GameCamera {
private:
    float m_mouseWheelMax{};
    float m_mouseWheelMin{};
    float m_currentMouseZoom{};

public:
    Camera2D m_camera{};
    Vector2 m_cameraPos{};

    GameCamera(const float mouseWheelMax, const float mouseWheelMin, const float currentMouseZoom)
    {
        m_mouseWheelMax = mouseWheelMax;
        m_mouseWheelMin = mouseWheelMin;
        m_currentMouseZoom = currentMouseZoom;
    }

    void Init()
    {
        m_camera.target = Vector2{ (float)GetScreenWidth()/2.0f, (float)GetScreenHeight()/2.0f };
        m_camera.offset = Vector2{ (float)GetScreenWidth()/2.0f, (float)GetScreenHeight()/2.0f };
        m_camera.rotation = 0.0f;
    }

    void Update()
    {
        m_cameraPos = m_camera.target - m_camera.offset;

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/m_camera.zoom);
            m_camera.target = Vector2Add(m_camera.target, delta);
        }

        m_currentMouseZoom += GetMouseWheelMove() * ((sin(m_currentMouseZoom)/2 + 0.5f)/(m_currentMouseZoom * 10));
        if (m_currentMouseZoom >= m_mouseWheelMax)
            m_currentMouseZoom = m_mouseWheelMax;
        if (m_currentMouseZoom <= m_mouseWheelMin)
            m_currentMouseZoom = m_mouseWheelMin;

        m_camera.zoom = m_currentMouseZoom;
        m_camera.offset = Vector2{ (float)GetScreenWidth()/2.0f, (float)GetScreenHeight()/2.0f};
    }

    float GetCurrentMouseZoom() const
    {
        return m_currentMouseZoom;
    }
};

#endif //GAMECAMERA_H
