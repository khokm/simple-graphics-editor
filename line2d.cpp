#include "line2d.h"

Line2D::Line2D()
{
}

Line2D::Line2D(float x1, float y1, float x2, float y2) :
    Line2D(Point2D(x1, y1), Point2D(x2, y2))
{
}

Line2D::Line2D(const Point2D &a, const Point2D &b)
{
    this->a = a;
    this->b = b;
}

Line2D Line2D::Transfer(const Line2D &line, const Point2D offset)
{
    return Line2D(line.a + offset, line.b + offset);
}

Line2D Line2D::Rotate(const Line2D &line, const Point2D &around, float angle)
{
    return Line2D(Point2D::RotatePoint(line.a, around, angle),
                  Point2D::RotatePoint(line.b, around, angle));
}

Line2D Line2D::Scale(const Line2D &line, const Point2D &around, const Point2D &multiplier)
{
    return Line2D(line.a + (line.a - around) * multiplier, line.b + (line.b - around) * multiplier);
}

float Line2D::Length() const
{
    return (b - a).Length();
}
