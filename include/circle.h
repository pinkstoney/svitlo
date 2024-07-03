#pragma once

#include "raylib.h"

class Circle
{
public:
    Circle(Vector2 center, float radius, float innerRadius);

public:
    Vector2 getCenter() const;
    float getRadius() const;
    float getInnerRadius() const ;

public:
    void draw(Color color, float startAngle, float midAngle) const;
    bool isMouseOver(float startAngle, float endAngle, float angle, Vector2 mousePosition) const;

private:
    Vector2 m_center;
    float m_radius;
    float m_innerRadius;
};
