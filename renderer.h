#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include "point2d.h"
#include "line2d.h"
#include <vector>
#include "mainwindow.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>

class Renderer : public QOpenGLWidget
{
    Q_OBJECT


public:
    explicit Renderer(QWidget *parent = 0);
    ~Renderer();

    void SetScale(float scale);
    void SetView(float x, float y);

    void LoadLines(const std::vector<Line2D> &lines);
    void LoadColors(const std::vector<Point2D> &colors);

    void SetLineWidth(float value);
    float GetLineWidth() const;

    void SetScene(class Scene *scene);
    Scene *GetScene() const;

    void SetMorphingState(float value);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

    void keyPressEvent(QKeyEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    float redColor[2] = {1, 1};

    Point2D FromSceneCoords(float x, float y) const;
    Point2D GetCursorPosition() const;
    QMatrix4x4 GetMatrix() const;
    void MousePress(QMouseEvent *event, bool singleClick);

    class Scene *defaultScene, *scene;

    QPoint previousCursorPoint;
    bool mouseHold;

    const float maxZoomAcceleration = 2;
    const float minZoomAcceleration = 0.5f;

    class MainWindow *mainWindow;

    QOpenGLShaderProgram m_program;
    int m_matrixUniform, m_vertAttrib, m_colorAttrib;

    QMatrix4x4 projectionMatrix, viewMatrix;

    int vertsCount;
    float *linesP, *colorsP;
};

#endif // RENDERER_H
