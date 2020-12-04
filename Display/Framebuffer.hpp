#pragma once

#include <cstdint>

#include "Point.hpp"
#include "Dimensions.hpp"
#include "Color565.hpp"

template<size_t width, size_t height>
class Framebuffer
{
public:

    Framebuffer(Point origin) : 
        _dimensions({width, height}),
        _origin(origin),
        _cursorPosition(origin)
    {}

    Framebuffer(Framebuffer const& other) = delete;
    Framebuffer(Framebuffer &&other) = delete;
    Framebuffer &operator=(Framebuffer const& other) = delete;
    Framebuffer &operator=(Framebuffer &&other) = delete;

    uint16_t operator[](size_t index) const
    {
        if (index >= width * height)
            return 0;
        return _framebuffer[index];
    }

    const uint16_t * GetRawBuffer() const
    {
        return _framebuffer;
    }

    void FillRectangle(Point origin, Dimensions dimensions, uint16_t color)
    {
        SetCursorPosition(origin);
        auto flattenedOrigin = _cursorPosition.X +
            _cursorPosition.Y * _dimensions.Width;
        for (uint16_t i = 0; i < dimensions.Height; ++i)
        {
            auto flattenedPosition = flattenedOrigin + 
                i * _dimensions.Width;
            for (uint16_t j = 0; j < dimensions.Width; ++j)
                _framebuffer[flattenedPosition++] = color;
        }
    }

    void FillRectangle(Point origin, Dimensions dimensions, Color565 color)
    {
        FillRectangle(origin, dimensions, static_cast<uint16_t>(color));
    }

    void FillBackground(uint16_t color)
    {
        FillRectangle(_origin, _dimensions, color);
    }

    void FillBackground(Color565 color)
    {
        FillBackground(static_cast<uint16_t>(color));
    }

    void DrawPoint(uint16_t color, Point position)
    {
        _framebuffer[position.Y * _dimensions.Width + position.X] = color;
    }

    void DrawPoint(Color565 color, Point position)
    {
        DrawPoint(static_cast<uint16_t>(color), position);
    }

    void DrawLine(uint16_t color, Point start, Point end)
    {
        bool inverted = false;

        int dx = end.X - start.X;
        int dy = end.Y - start.Y;

        if (std::abs(dx) < std::abs(dy))
        {
            std::swap(start.X, start.Y);
            std::swap(end.X, end.Y);
            dx = end.X - start.X;
            dy = end.Y - start.Y;
            inverted = true;
        }

        if (dx < 0)
        {
            std::swap(start, end);
            dx = -dx;
            dy = -dy;
        }

        int normalizedSlope = 0;
        int normalizedError = 0;
        
        if (dx != 0)
            normalizedSlope = std::abs(dy) * 2;

        uint16_t y = start.Y;

        for (uint16_t x = start.X; x <= end.X; ++x)
        {
            auto flattenedPosition = (inverted == true) ?
                x * _dimensions.Width + y :
                y * _dimensions.Width + x;
            _framebuffer[flattenedPosition] = color;

            normalizedError += normalizedSlope;
            if (normalizedError > dx)
            {
                normalizedError -= dx * 2;
                y += ((dy > 0) ? 1 : -1);
            }
        }
    }
    
    void DrawLine(Color565 color, Point start, Point end)
    {
        DrawLine(static_cast<uint16_t>(color), start, end);
    }

    void DrawTriangle(uint16_t color, Point p1, Point p2, Point p3)
    {
        DrawLine(color, p1, p2);
        DrawLine(color, p2, p3);
        DrawLine(color, p3, p1);
    }

    void DrawTriangle(Color565 color, Point p1, Point p2, Point p3)
    {
        DrawTriangle(static_cast<uint16_t>(color), p1, p2, p3);
    }

    void DrawSymbol(uint8_t const *symbolMap,
        Point origin, Dimensions dimensions,
        uint16_t backgroundColor, uint16_t foregroundColor)
    {
        SetCursorPosition(origin);
        auto flattenedOrigin = _cursorPosition.Y * _dimensions.Width +
            _cursorPosition.X;
        for (uint16_t i = 0; i < dimensions.Height; ++i)
        {
            auto flattenedPosition = flattenedOrigin +
                i * _dimensions.Width;
            for (uint16_t j = 0; j < dimensions.Width; ++j)
                _framebuffer[flattenedPosition++] =
                    (symbolMap[i * dimensions.Width + j]) ?
                        foregroundColor :
                        backgroundColor;
        }
    }

    void DrawSymbol(uint8_t const *symbolMap,
        Point origin, Dimensions dimensions,
        Color565 backgroundColor, Color565 foregroundColor)
    {
        DrawSymbol(symbolMap, origin, dimensions,
            static_cast<uint16_t>(backgroundColor),
            static_cast<uint16_t>(foregroundColor));
    }

private:

    void SetCursorPosition(Point newPosition)
    {
        _cursorPosition = newPosition;
    }

    uint16_t _framebuffer[width * height];
    Dimensions _dimensions;
    Point _origin;
    Point _cursorPosition;
};
