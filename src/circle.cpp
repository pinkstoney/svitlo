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
    auto distance = static_cast<float>(std::sqrt(std::pow(mousePosition.x - m_center.x, 2) + std::pow(mousePosition.y - m_center.y, 2)));
    return startAngle <= angle && angle < endAngle && distance > m_innerRadius && distance < m_radius;
}