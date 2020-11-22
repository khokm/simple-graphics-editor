#ifndef FRACTALGLWIDGET_H
#define FRACTALGLWIDGET_H
#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <vector>
#include "line2d.h"

class FractalGLWidget : public QOpenGLWidget
{

    Q_OBJECT

public:
    explicit FractalGLWidget(QWidget *parent);
    ~FractalGLWidget();

    void BuildTree(int deep, int branchCount, bool firstRight, float branchAngle, int &totalCount);

    std::vector<Line2D> lines;
    std::vector<Point2D> colors;

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    float minY, maxY, minX, maxX;

    const Point2D defaultLineColor = Point2D(1, 1);

    void Clear();
    QOpenGLShaderProgram m_program;
    int m_matrixUniform, m_vertAttrib, m_colorAttrib;

    void CreateBranch(const Point2D &start, const Point2D &end, int currentDeep, int maxDeep, int branchCount, bool rightBranch, float branchAngle, int &totalCount);
};

#endif // FRACTALGLWIDGET_H
