#include "../include/circle.h"
#include <cmath>

Circle::Circle(Vector2 center, float radius, float innerRadius)
    : m_center(center), m_radius(radius), m_innerRadius(innerRadius) {}

Vector2 Circle::getCenter() const { return m_center; }
float Circle::getRadius() const { return m_radius; }
float Circle::getInnerRadius() const  { return m_innerRadius; }

void Circle::draw(Color color, float startAngle, float midAngle) const
{
    DrawRing(m_center, m_innerRadius, m_radius, startAngle, midAngle, 60, color);
}

bool Circle::isMouseOver(float startAngle, float endAngle, float angle, Vector2 mousePosition) const
{
    float dx = mousePosition.x - m_center.x;
    float dy = mousePosition.y - m_center.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    return startAngle <= angle && angle < endAngle && distance > m_innerRadius && distance < m_radius;
}

float Circle::calculateAngle(int index, float anglePerSegment) const
{
    return std::fmod(static_cast<float>(index) * anglePerSegment - 90.0f + 360.0f, 360.0f);}

float Circle::calculateMidAngle(float startAngle, float anglePerSegment) const
{
    return std::fmod(startAngle + (anglePerSegment - 0.5f), 360.0f);
}

float Circle::calculateEndAngle(int index, float anglePerSegment) const
{
    return std::fmod((static_cast<float>(index) + 1) * anglePerSegment - 90.0f + 360.0f, 360.0f);
}

std::tuple<float, float, float> Circle::calculateAngles(int index, float anglePerSegment) const
{
    float startAngle = calculateAngle(index, anglePerSegment);
    float midAngle = calculateMidAngle(startAngle, anglePerSegment);
    float endAngle = calculateEndAngle(index, anglePerSegment);
    return {startAngle, midAngle, endAngle};
}

Vector2 Circle::calculateTextPosition(float angle, float radius) const
{
    return {
        m_center.x + radius * std::cos(angle * DEG2RAD),
        m_center.y + radius * std::sin(angle * DEG2RAD)
    };
}

float Circle::calculateTextAngle(float startAngle, float endAngle) const
{
    float textAngle = (startAngle > endAngle) ? (startAngle + (endAngle + 360)) / 2 : (startAngle + endAngle) / 2;
    return std::fmod(textAngle + 360.0f, 360.0f);
}

bool Circle::isMouseOverSegment(float startAngle, float endAngle, const Vector2& mousePosition) const
{
    float mouseAngle = std::fmod(std::atan2(mousePosition.y - m_center.y, mousePosition.x - m_center.x) * RAD2DEG + 360.0f, 360.0f);
    return startAngle <= mouseAngle && mouseAngle <= endAngle && 
           CheckCollisionPointCircle(mousePosition, m_center, m_radius);
}
