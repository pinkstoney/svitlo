#include "../include/shutdown-circle.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

constexpr float ANGLE_PER_HOUR = 360.0f / 24.0f;
constexpr Color CUSTOM_GREEN = {161, 221, 112, 255};
constexpr Color CUSTOM_RED = {238, 78, 78, 255};
constexpr Color CUSTOM_ORANGE = {253, 208, 157, 255};
constexpr Color DEFAULT_COLOR = LIGHTGRAY;

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

ShutdownCircle::ShutdownCircle(Vector2 center, float radius, float innerRadius,
                               ElectricityData data, const Font& font)
        : Circle(center, radius, innerRadius),
          m_data(std::move(data)),
          m_font(font),
          m_segmentSizes(24, radius)
{
}

float ShutdownCircle::m_calculateAngle(int i) const
{
    return std::fmod(static_cast<float>(i) * ANGLE_PER_HOUR - 90.0f + 360.0f, 360.0f);
}

float ShutdownCircle::m_calculateMidAngle(float startAngle) const
{
    return std::fmod(startAngle + (ANGLE_PER_HOUR - 0.5f), 360.0f);
}

float ShutdownCircle::m_calculateEndAngle(int i) const
{
    return std::fmod((static_cast<float>(i) + 1) * ANGLE_PER_HOUR - 90.0f + 360.0f, 360.0f);
}

std::tuple<float, float, float> ShutdownCircle::m_calculateAngles(int i) const
{
    float startAngle = m_calculateAngle(i);
    float midAngle = m_calculateMidAngle(startAngle);
    float endAngle = m_calculateEndAngle(i);

    return {startAngle, midAngle, endAngle};
}

bool ShutdownCircle::m_isHourInPeriod(int hour, const std::pair<int, int>& period) const
{
    return (period.first <= period.second)
           ? period.first <= hour && hour < period.second
           : hour < period.second || period.first <= hour;
}

bool ShutdownCircle::m_isColorForHour(int hour, const std::vector<std::pair<int, int>>& periods) const
{
    return std::any_of(periods.begin(), periods.end(), [hour, this](const std::pair<int, int>& p)
    {
        return m_isHourInPeriod(hour, p);
    });
}

Color ShutdownCircle::m_determineColor(int i) const
{
    int hour = (i + 1) % 24;
    hour = hour == 0 ? 24 : hour;

    if (m_isColorForHour(hour, m_data.getWillBeElectricity())) return CUSTOM_GREEN;
    if (m_isColorForHour(hour, m_data.getMightBeElectricity())) return CUSTOM_ORANGE;
    if (m_isColorForHour(hour, m_data.getWontBeElectricity())) return CUSTOM_RED;

    return DEFAULT_COLOR;
}

float ShutdownCircle::m_calculateTextAngle(float startAngle, float endAngle) const
{
    float textAngle;
    if (startAngle > endAngle)
        textAngle = (startAngle + (endAngle + 360)) / 2;
    else
        textAngle = (startAngle + endAngle) / 2;

    return std::fmod(textAngle + 360.0f, 360.0f);
}

Vector2 ShutdownCircle::m_calculateTextPosition(float angle, float radius) const
{
    return
    {
        getCenter().x + radius * std::cos(angle * PI / 180.0f),
        getCenter().y + radius * std::sin(angle * PI / 180.0f)
    };
}

std::string ShutdownCircle::m_formatHourText(int i) const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << i << "-" << std::setw(2) << std::setfill('0') << (i + 1) % 24;
    return oss.str();
}

void ShutdownCircle::m_drawFormattedText(const std::string& text, Vector2 position, int size, Color color) const
{
    Vector2 textSize = MeasureTextEx(m_font, text.c_str(), static_cast<float>(size), 1);
    Vector2 textPosition = {position.x - textSize.x / 2.0f, position.y - textSize.y / 2.0f};
    DrawTextEx(m_font, text.c_str(), textPosition, static_cast<float>(size), 1, color);
}

void ShutdownCircle::m_drawHourText(int i, float startAngle, float endAngle, int size) const
{
    float textAngle = m_calculateTextAngle(startAngle, endAngle);
    float textRadius = (getInnerRadius() + getRadius()) / 2;
    Vector2 textPos = m_calculateTextPosition(textAngle, textRadius);
    std::string hourText = m_formatHourText(i);

    m_drawFormattedText(hourText, textPos, size, DARKGRAY);
}

void ShutdownCircle::drawHourSegments()
{
    for (int i = 0; i < 24; ++i)
    {
        drawSegment(i);
        drawHourText(i);
    }
    drawQueueSubqueueText();
}

bool ShutdownCircle::m_isMouseOver(float startAngle, float endAngle, float mouseAngle, const Vector2& mousePosition) const
{
    return startAngle <= mouseAngle && mouseAngle <= endAngle && CheckCollisionPointCircle(mousePosition, getCenter(), getRadius());
}

void ShutdownCircle::m_updateSegmentSize(int i, float mouseAngle, const Vector2& mousePosition)
{
    auto [startAngle, midAngle, endAngle] = m_calculateAngles(i);

    if (m_isMouseOver(startAngle, endAngle, mouseAngle, mousePosition))
        m_segmentSizes[i] = lerp(m_segmentSizes[i], getRadius() + 20.0f, 0.2f);
    else
        m_segmentSizes[i] = lerp(m_segmentSizes[i], getRadius(), 0.1f);
}

void ShutdownCircle::m_drawSegmentRing(int i, float startAngle, float midAngle, Color color) const
{
    DrawRing(getCenter(), getInnerRadius(), m_segmentSizes[i], startAngle, midAngle, 60, color);
}

void ShutdownCircle::drawSegment(int i)
{
    auto [startAngle, midAngle, endAngle] = m_calculateAngles(i);
    Color color = m_determineColor(i);

    Vector2 mousePosition = GetMousePosition();
    float mouseAngle = std::fmod(std::atan2(mousePosition.y - getCenter().y, mousePosition.x - getCenter().x) * 180.0f / PI + 360.0f, 360.0f);

    m_updateSegmentSize(i, mouseAngle, mousePosition);
    m_drawSegmentRing(i, startAngle, midAngle, color);
}

void ShutdownCircle::drawHourText(int i) const
{
    auto [startAngle, midAngle, endAngle] = m_calculateAngles(i);
    m_drawHourText(i, startAngle, endAngle, 15);
}

void ShutdownCircle::drawQueueSubqueueText() const
{
    std::string queueSubqueueText = std::to_string(m_data.getQueue()) + "." + std::to_string(m_data.getSubqueue());
    Vector2 textSize = MeasureTextEx(m_font, queueSubqueueText.c_str(), 50, 1);
    Vector2 textPosition = {getCenter().x - textSize.x / 2.0f, getCenter().y - textSize.y / 2.0f};
    DrawTextEx(m_font, queueSubqueueText.c_str(), textPosition, 50, 1, BLACK);
}