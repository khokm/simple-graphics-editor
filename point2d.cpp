#include "point2d.h"

Point2D Point2D::RotatePoint(const Point2D &point, const Point2D &around, float angle)
{
    float cosA = cos(angle);
    float sinA = sin(angle);

    return Point2D(
                around.x + (point.x - around.x) * cosA - (point.y - around.y) * sinA,
                around.y + (point.y - around.y) * cosA + (point.x - around.x) * sinA
                );
}

Point2D::Point2D()
{
    x = 0;
    y = 0;
}

Point2D::Point2D(float x, float y)
{
    this->x = x;
    this->y = y;
}

float Point2D::Length() const
{
    return FastSqr(sqrLength());
}

float Point2D::sqrLength() const
{
    return x * x + y * y;
}

Point2D Point2D::normalized() const
{
    return (*this) / Length();
}

Point2D Point2D::operator +=(const Point2D &v1)
{
    this->x += v1.x;
    this->y += v1.y;
    return *this;
}

Point2D Point2D::operator -=(const Point2D &v1)
{
    return (*this) += -v1;
}

Point2D Point2D::operator *=(const QMatrix4x4 &matrix)
{
    QVector4D result = matrix * QVector4D(this->x, this->y, 0, 1);

    this->x = result.x();
    this->y = result.y();

    return (*this);
}

Point2D operator -(const Point2D &v1, const Point2D &v2)
{
    return Point2D(v1.x - v2.x, v1.y - v2.y);
}

Point2D operator /(const Point2D &v1, float &v2)
{
    return Point2D(v1.x / v2, v1.y / v2);
}

Point2D operator *(const Point2D &v1, float multiplier)
{
    return Point2D(v1.x * multiplier, v1.y * multiplier);
}

Point2D operator +=(Point2D &v1, Point2D &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;

    return v1;
}

Point2D operator +(const Point2D &v1, const Point2D &v2)
{
    return Point2D(v1.x + v2.x, v1.y + v2.y);
}

Point2D operator /(const Point2D &v1, float denum)
{
    return Point2D(v1.x / denum, v1.y / denum);
}

Point2D operator -(const Point2D &v1)
{
    return Point2D(-v1.x, -v1.y);
}

Point2D operator *(const Point2D &v1, const Point2D &v2)
{
    return Point2D(v1.x * v2.x, v1.y * v2.y);
}
