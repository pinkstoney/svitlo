#include "../include/shutdown-circle.h"
#include <algorithm>
#include <cstdio>

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

ShutdownCircle::ShutdownCircle(Vector2 center, float radius, float innerRadius,
                               std::vector<std::pair<int, int>> willBeElectricity,
                               std::vector<std::pair<int, int>> mightBeElectricity,
                               std::vector<std::pair<int, int>> wontBeElectricity,
                               Font font, int queue, int subqueue)
        : Circle(center, radius, innerRadius),
          m_willBeElectricity(std::move(willBeElectricity)),
          m_mightBeElectricity(std::move(mightBeElectricity)),
          m_wontBeElectricity(std::move(wontBeElectricity)),
          m_font(font),
          m_segmentSizes(24, radius),
          m_queue(queue),
          m_subqueue(subqueue)
{
}

std::tuple<float, float, float> ShutdownCircle::m_calculateAngles(int i) const
{
    float startAngle = static_cast<float>(i) * 360.0f / 24 - 90;
    if (startAngle < 0)
        startAngle += 360;

    float midAngle = startAngle + ((360.0f / 24) - 0.5f);

    float endAngle = (static_cast<float>(i) + 1) * 360.0f / 24 - 90;
    if (endAngle <= 0)
        endAngle += 360;

    return {startAngle, midAngle, endAngle};
}

Color ShutdownCircle::m_determineColor(int i)
{
    Color color = LIGHTGRAY;
    Color green = { 161, 221, 112, 255 };
    Color red = { 238, 78, 78, 255 };
    Color orange = { 253, 208, 157, 255 };

    int hour = (i + 1) % 24;
    if (hour == 0) hour = 24;

    auto willBe = std::find_if(m_willBeElectricity.begin(), m_willBeElectricity.end(), [hour](const std::pair<int, int>& p)
    {
        if (p.first <= p.second) return p.first <= hour && hour < p.second;
        else return hour < p.second || p.first <= hour;
    });

    auto mightBe = std::find_if(m_mightBeElectricity.begin(), m_mightBeElectricity.end(), [hour](const std::pair<int, int>& p)
    {
        if (p.first <= p.second) return p.first <= hour && hour < p.second;
        else return hour < p.second || p.first <= hour;
    });

    auto wontBe = std::find_if(m_wontBeElectricity.begin(), m_wontBeElectricity.end(), [hour](const std::pair<int, int>& p)
    {
        if (p.first <= p.second) return p.first <= hour && hour < p.second;
        else return hour < p.second || p.first <= hour;
    });

    if (willBe != m_willBeElectricity.end())
        color = green;
    else if (mightBe != m_mightBeElectricity.end())
        color = orange;
    else if (wontBe != m_wontBeElectricity.end())
        color = red;

    return color;
}

void ShutdownCircle::m_drawHourText(int i, float startAngle, float endAngle, int size)
{
    float textAngle = (startAngle + endAngle) / 2;
    if (textAngle < 0)
        textAngle += 360;

    float textRadius = (getInnerRadius() + getRadius()) / 2;
    Vector2 textPos = { getCenter().x + textRadius * cos(textAngle * PI / 180.0f), getCenter().y + textRadius * sin(textAngle * PI / 180.0f) };

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << i << "-" << std::setw(2) << std::setfill('0') << (i + 1) % 24;
    std::string hourText = oss.str();

    Vector2 textSize = MeasureTextEx(m_font, hourText.c_str(), static_cast<float>(size), 1);

    Vector2 textPosition = { textPos.x - textSize.x / 2.0f, textPos.y - textSize.y / 2.0f };
    DrawTextEx(m_font, hourText.c_str(), textPosition, static_cast<float>(size), 1, DARKGRAY);
}


void ShutdownCircle::drawHourSegments()
{
    for (int i = 0; i < 24; i++)
    {
        drawSegment(i);
        drawHourText(i);
    }
    drawQueueSubqueueText();
}

void ShutdownCircle::drawSegment(int i)
{
    auto [startAngle, midAngle, endAngle] = m_calculateAngles(i);
    Color color = m_determineColor(i);

    Vector2 mousePosition = GetMousePosition();
    float angle = atan2(mousePosition.y - getCenter().y, mousePosition.x - getCenter().x) * 180.0f / PI;
    if (angle < 0) angle += 360;

    if (isMouseOver(startAngle, endAngle, angle, mousePosition))
        m_segmentSizes[i] = lerp(m_segmentSizes[i], getRadius() + 20, 0.2f);
    else
        m_segmentSizes[i] = lerp(m_segmentSizes[i], getRadius(), 0.1f);

    DrawRing(getCenter(), getInnerRadius(), m_segmentSizes[i], startAngle, midAngle, 60, color);
}

void ShutdownCircle::drawHourText(int i)
{
    auto [startAngle, midAngle, endAngle] = m_calculateAngles(i);
    m_drawHourText(i, startAngle, endAngle, 15);
}

void ShutdownCircle::drawQueueSubqueueText()
{
    std::string queueSubqueueText = std::to_string(m_queue) + "." + std::to_string(m_subqueue);

    Vector2 textSize = MeasureTextEx(m_font, queueSubqueueText.c_str(), 50, 1);
    Vector2 textPosition = { getCenter().x - textSize.x / 2.0f, getCenter().y - textSize.y / 2.0f };

    DrawTextEx(m_font, queueSubqueueText.c_str(), textPosition, 50, 1, BLACK);
}
