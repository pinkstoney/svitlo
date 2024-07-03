#pragma once

#include <tuple>

#include "raylib.h"

class Circle 
{
protected:
    Vector2 m_center;
    float m_radius;
    float m_innerRadius;

public:
    Circle(Vector2 center, float radius, float innerRadius);
   
public:
    Vector2 getCenter() const;
    float getRadius() const;
    float getInnerRadius() const;
   
public:
    void draw(Color color, float startAngle, float midAngle) const;
    bool isMouseOver(float startAngle, float endAngle, float angle, Vector2 mousePosition) const;
    bool isMouseOverSegment(float startAngle, float endAngle, const Vector2& mousePosition) const;

public:
    float calculateAngle(int index, float anglePerSegment) const;
    float calculateMidAngle(float startAngle, float anglePerSegment) const;
    float calculateEndAngle(int index, float anglePerSegment) const;
    std::tuple<float, float, float> calculateAngles(int index, float anglePerSegment) const;

    
    Vector2 calculateTextPosition(float angle, float radius) const;
    float calculateTextAngle(float startAngle, float endAngle) const;
};
