#pragma once

#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "circle.h"
#include "electricity-data.h"

class ShutdownCircle : public Circle
{
public:
    ShutdownCircle(Vector2 center, float radius, float innerRadius,
                   ElectricityData data, const Font& font);

public:
    void drawHourSegments();
    void drawSegment(int i);
    void drawHourText(int i) const;
    void drawQueueSubqueueText() const;

private:
    Color m_determineColor(int i) const;
    bool m_isHourInPeriod(int hour, const std::pair<int, int>& period) const;
    bool m_isColorForHour(int hour, const std::vector<std::pair<int, int>>& periods) const;
    bool m_isMouseOver(float startAngle, float endAngle, float mouseAngle, const Vector2& mousePosition) const;

private:
    void m_drawHourText(int i, float startAngle, float endAngle, int size) const;
    void m_drawSegmentRing(int i, float startAngle, float midAngle, Color color) const;
    void m_drawFormattedText(const std::string& text, Vector2 position, int size, Color color) const;

    std::string m_formatHourText(int i) const;
    void m_updateSegmentSize(int i, float mouseAngle, const Vector2& mousePosition);

private:
    Vector2 m_calculateTextPosition(float angle, float radius) const;

    std::tuple<float, float, float> m_calculateAngles(int i) const;
    float m_calculateTextAngle(float startAngle, float endAngle) const;
    float m_calculateAngle(int i) const;
    float m_calculateMidAngle(float startAngle) const;
    float m_calculateEndAngle(int i) const;

private:
    ElectricityData m_data;
    Font m_font;
    std::vector<float> m_segmentSizes;
};