#include "../include/shutdown-circle.h"

#include "../include/utility.h"
#include "../include/ui-manager.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <iomanip>

ShutdownCircle::ShutdownCircle(Vector2 center, float radius, float innerRadius,
                               ElectricityData data, const Font& font)
    : Circle(center, radius, innerRadius),
      m_data(std::move(data)),
      m_font(font),
      m_segmentSizes(24, radius)
{
}

bool ShutdownCircle::m_isHourInPeriod(int hour, const std::pair<int, int>& period) const
{
    if (period.first <= period.second)
        return period.first <= hour && hour < period.second;
    return hour < period.second || period.first <= hour;
}

bool ShutdownCircle::m_isColorForHour(int hour, const std::vector<std::pair<int, int>>& periods) const
{
    return std::any_of(periods.begin(), periods.end(), 
        [this, hour](const auto& p) { return m_isHourInPeriod(hour, p); });
}

Color ShutdownCircle::m_determineColor(int i) const
{
    int hour = (i + 0) % 24;
    hour = hour == 0 ? 24 : hour;

    if (m_isColorForHour(hour, m_data.getWillBeElectricity())) return CUSTOM_GREEN;
    if (m_isColorForHour(hour, m_data.getMightBeElectricity())) return CUSTOM_ORANGE;
    if (m_isColorForHour(hour, m_data.getWontBeElectricity())) return CUSTOM_RED;
    return DEFAULT_COLOR;
}

std::string ShutdownCircle::m_formatHourText(int i) const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << i << "-" 
        << std::setw(2) << std::setfill('0') << (i + 1) % 24;
    return oss.str();
}

void ShutdownCircle::m_drawFormattedText(const std::string& text, Vector2 position, float size, Color color) const
{
    Vector2 textSize = MeasureTextEx(m_font, text.c_str(), size, 1);
    Vector2 textPosition = 
    {
        position.x - textSize.x / 2.0f,
        position.y - textSize.y / 2.0f
    };
    DrawTextEx(m_font, text.c_str(), textPosition, size, 1, color);
}

void ShutdownCircle::m_drawHourText(int i, float startAngle, float endAngle, float size) const
{
    float textAngle = calculateTextAngle(startAngle, endAngle);
    float textRadius = (getInnerRadius() + getRadius()) / 2;
    Vector2 textPos = calculateTextPosition(textAngle, textRadius);
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

void ShutdownCircle::m_updateSegmentSize(int i, const Vector2& mousePosition)
{
    auto [startAngle, midAngle, endAngle] = calculateAngles(i, ANGLE_PER_HOUR);
    if (isMouseOverSegment(startAngle, endAngle, mousePosition))
        m_segmentSizes[i] = Utility::lerp(m_segmentSizes[i], getRadius() + 20.0f, 0.2f);
    else
        m_segmentSizes[i] = Utility::lerp(m_segmentSizes[i], getRadius(), 0.1f);
}

void ShutdownCircle::m_drawSegmentRing(int i, float startAngle, float midAngle, Color color) const
{
    DrawRing(getCenter(), getInnerRadius(), m_segmentSizes[i], startAngle, midAngle, 60, color);
}

void ShutdownCircle::drawSegment(int i)
{
    auto [startAngle, midAngle, endAngle] = calculateAngles(i, ANGLE_PER_HOUR);
    Color color = m_determineColor(i);
    Vector2 mousePosition = GetMousePosition();
    m_updateSegmentSize(i, mousePosition);
    m_drawSegmentRing(i, startAngle, midAngle, color);
}

void ShutdownCircle::drawHourText(int i) const
{
    auto [startAngle, midAngle, endAngle] = calculateAngles(i, ANGLE_PER_HOUR);
    m_drawHourText(i, startAngle, endAngle, 15);
}

void ShutdownCircle::drawQueueSubqueueText() const
{
    std::string queueSubqueueText = std::to_string(m_data.getQueue()) + "." + std::to_string(m_data.getSubqueue());
    Vector2 textSize = MeasureTextEx(m_font, queueSubqueueText.c_str(), 50, 1);
    Vector2 textPosition = 
    {
        getCenter().x - textSize.x / 2.0f,
        getCenter().y - textSize.y / 2.0f
    };
    DrawTextEx(m_font, queueSubqueueText.c_str(), textPosition, 50, 1, BLACK);
}
