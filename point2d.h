#ifndef POINT2D_H
#define POINT2D_H
#include <cmath>
#include <float_math.h>
#include <QMatrix4x4>

struct Point2D
{
    float x, y;
    Point2D();
    Point2D(float x, float y);

    float Length() const;
    float sqrLength() const;

    Point2D normalized() const;
    Point2D operator += (const Point2D &v1);
    Point2D operator -= (const Point2D &v1);
    Point2D operator *= (const QMatrix4x4 &matrix);

    static Point2D RotatePoint(const Point2D &point, const Point2D &around, float angle);

};

Point2D operator + (const Point2D &v1, const Point2D &v2);
Point2D operator - (const Point2D &v1, const Point2D &v2);
Point2D operator - (const Point2D &v1);
Point2D operator / (const Point2D &v1, float denum);
Point2D operator * (const Point2D &v1, float multiplier);
Point2D operator * (const Point2D &v1, const Point2D &v2);


#endif // POINT2D_H
