#ifndef LINE2D_H
#define LINE2D_H
#include <cmath>
#include "point2d.h"

struct Line2D
{
    Point2D a, b;

    Line2D();
    Line2D(float x1, float y1, float x2, float y2);
    Line2D(const Point2D &a, const Point2D &b);

    static Line2D Transfer   (const Line2D &line, const Point2D offset);
    static Line2D Rotate     (const Line2D &line, const Point2D &around, float angle);
    static Line2D Scale      (const Line2D &line, const Point2D &around, const Point2D &multiplier);

    float Length() const;
};

#endif // LINE2D_H
