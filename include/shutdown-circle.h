#pragma once

#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>

#include "circle.h"

class ShutdownCircle : public Circle
{
public:
    ShutdownCircle(Vector2 center, float radius, float innerRadius,
                   std::vector<std::pair<int, int>> willBeElectricity,
                   std::vector<std::pair<int, int>> mightBeElectricity,
                   std::vector<std::pair<int, int>> wontBeElectricity,
                   Font font, int queue, int subqueue);


public:
    void drawHourSegments();
    void drawSegment(int i);
    void drawHourText(int i);
    void drawQueueSubqueueText();

private:
    std::vector<std::pair<int, int>> m_willBeElectricity;
    std::vector<std::pair<int, int>> m_mightBeElectricity;
    std::vector<std::pair<int, int>> m_wontBeElectricity;

    Font m_font;

private:
    int m_queue;
    int m_subqueue;

    std::vector<float> m_segmentSizes;

    std::tuple<float, float, float> m_calculateAngles(int i) const;
    Color m_determineColor(int i);
    void m_drawHourText(int i, float startAngle, float endAngle, int size);
};
