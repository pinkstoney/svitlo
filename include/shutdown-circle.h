#pragma once

#include <vector>
#include <utility>

#include "circle.h"
#include "electricity-data.h"

class ShutdownCircle : public Circle 
{
private:
    ElectricityData m_data;
    Font m_font;
    std::vector<float> m_segmentSizes;

    bool m_isHourInPeriod(int hour, const std::pair<int, int>& period) const;
    bool m_isColorForHour(int hour, const std::vector<std::pair<int, int>>& periods) const;
    Color m_determineColor(int i) const;
    std::string m_formatHourText(int i) const;
    void m_drawFormattedText(const std::string& text, Vector2 position, float size, Color color) const;
    void m_drawHourText(int i, float startAngle, float endAngle, float size) const;
    void m_updateSegmentSize(int i, const Vector2& mousePosition);
    void m_drawSegmentRing(int i, float startAngle, float midAngle, Color color) const;

public:
    ShutdownCircle(Vector2 center, float radius, float innerRadius,
                   ElectricityData data, const Font& font);

    void drawHourSegments();
    void drawSegment(int i);
    void drawHourText(int i) const;
    void drawQueueSubqueueText() const;
};
