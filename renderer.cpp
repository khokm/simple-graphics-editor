#include "renderer.h"

Renderer::Renderer(QWidget *parent) : QOpenGLWidget(parent)
{
    mainWindow = (MainWindow*)(parentWidget()->parentWidget());
    defaultScene = new Scene();
    scene = defaultScene;
}

Renderer::~Renderer()
{
    delete defaultScene;
}

void Renderer::LoadLines(const std::vector<Line2D> &lines)
{
    linesP = lines.size() > 0 ? (float*)(&lines[0]) : 0;

    vertsCount = lines.size() * 2;
}

void Renderer::LoadColors(const std::vector<Point2D> &colors)
{
    colorsP = colors.size() > 0 ? (float*)(&colors[0]) : 0;
}

void Renderer::SetLineWidth(float value)
{
    scene->lineWidth = value;
}

float Renderer::GetLineWidth() const
{
    return scene->lineWidth;
}

Point2D Renderer::FromSceneCoords(float x, float y) const
{
    QVector4D result = QVector4D(x, y, 0, 1) * GetMatrix();

    return Point2D(result.x(), result.y());
}

Point2D Renderer::GetCursorPosition() const
{
    QPoint p = mapFromGlobal(QCursor::pos());
    float x =   2.0f * p.x() / width()  - 1;
    float y =  -2.0f * p.y() / height() + 1;

    Point2D view = FromSceneCoords(scene->viewX, scene->viewY);

    QVector4D result = QVector4D(x - view.x, y - view.y, 0, 1) * GetMatrix().inverted();

    return Point2D{result.x(), result.y()};
}

void Renderer::SetScene(Scene *scene)
{
    this->scene = scene;

    scene->UnloadDataToRenderer(this);

    SetScale(scene->screenScale);
    SetView(scene->viewX, scene->viewY);
    SetLineWidth(scene->lineWidth);

    mouseReleaseEvent(0);
    mainWindow->UpdateViewScaleInput(scene->screenScale);
}

Scene *Renderer::GetScene() const
{
    return scene;
}

void Renderer::SetMorphingState(float value)
{
    scene->SetMorphingState(value);
    mouseReleaseEvent(0);
}

void Renderer::SetScale(float scale)
{
    if(scale < 0.01f)
        scale = 0.01f;
    if(scale > 5)
        scale = 5;

    scene->screenScale = scale;

    float aspectX = (height() < width() ? (float)width() / height() : 1) / scale;
    float aspectY = (height() > width() ? (float)height() / width() : 1) / scale;

    projectionMatrix = QMatrix();
    projectionMatrix.ortho(-aspectX, aspectX, -aspectY, aspectY, -1, 1);

    SetView(scene->viewX, scene->viewY);
}

void Renderer::SetView(float x, float y)
{
    scene->viewX = x;
    scene->viewY = y;

    viewMatrix = QMatrix();

    Point2D view = FromSceneCoords(x, y);

    viewMatrix.translate(view.x, view.y);
}

void Renderer::initializeGL()
{
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":/Shaders/vShader.glsl");

    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":/Shaders/fShader.glsl");

    m_program.addShader(&vShader);
    m_program.addShader(&fShader);

    if(!m_program.link())
    {
        qWarning("Shader error");
        exit(0);
    }

    m_matrixUniform = m_program.uniformLocation("matrix2D");
    m_vertAttrib = m_program.attributeLocation("vert2D");
    m_colorAttrib = m_program.attributeLocation("vertexColor");

    m_program.enableAttributeArray(m_vertAttrib);
    m_program.enableAttributeArray(m_colorAttrib);

    glClearColor(1, 1, 1, 1);

    if(!m_program.bind())
    {
        qWarning("shader program error");
        exit(0);
    }

}

void Renderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    SetScale(scene->screenScale);
    mainWindow->UpdateBottomPanelPosition();
}

void Renderer::paintGL()
{
    m_program.setUniformValue(m_matrixUniform, GetMatrix());
    glLineWidth(scene->lineWidth);
    glPointSize(scene->lineWidth * 2);

    glClear(GL_COLOR_BUFFER_BIT);

    if(linesP != 0)
    {
        m_program.setAttributeArray(m_vertAttrib, linesP, 2);
        m_program.setAttributeArray(m_colorAttrib, colorsP, 1);

        glDrawArrays(GL_LINES, 0, vertsCount);
        if(mainWindow->PointModeEnabled())
            glDrawArrays(GL_POINTS, 0, vertsCount);

    }

    if(scene->GetCreationMode() == CreationMode::SetLineEnd)
    {
        m_program.setAttributeArray(m_vertAttrib, scene->GetSpecialLineData(), 2);
        m_program.setAttributeArray(m_colorAttrib, redColor, 1);

        glDrawArrays(GL_LINES, 0, 2);
        glDrawArrays(GL_POINTS, 0, 2);

        if(linesP != 0)
        {
            m_program.setAttributeArray(m_vertAttrib, linesP, 2);
            m_program.setAttributeArray(m_colorAttrib, colorsP, 1);
        }
    }
}


QMatrix4x4 Renderer::GetMatrix() const
{
    return viewMatrix * projectionMatrix;
}

void Renderer::MousePress(QMouseEvent *event, bool singleClick)
{
    previousCursorPoint = QCursor::pos();

    mouseHold = true;

    if(!(event->buttons() & Qt::LeftButton) || (event->buttons() & Qt::RightButton))
        return;

    Point2D scenePoint = GetCursorPosition();

    switch (scene->GetCreationMode())
    {
    case CreationMode::SetLineStart:
        scene->SetCreateLineStart(scenePoint);
        break;
    case CreationMode::SetLineEnd:
        scene->SetCreateLineEnd(scenePoint, QApplication::keyboardModifiers() & Qt::ControlModifier);
        scene->UnloadDataToRenderer(this);
        break;
    default:
        bool pointMode = mainWindow->PointModeEnabled();
        if(pointMode)
            scene->SelectPointByTouch(scenePoint);
        else
            scene->SelectLineByTouch(scenePoint, QApplication::keyboardModifiers() & Qt::ControlModifier, singleClick);

        scene->SaveSelectionState(scenePoint, pointMode);
        break;
    }

    repaint();

}

void Renderer::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(scene->GetCreationMode() == DontCreate)
        MousePress(event, false);
}
void Renderer::mousePressEvent(QMouseEvent *event)
{
    MousePress(event, true);
}

void Renderer::mouseReleaseEvent(QMouseEvent *event)
{
    mouseHold = false;

    mainWindow->UpdateViewPositionInput(scene->viewX, scene->viewY);

    bool hasSelection = scene->HasSelection(mainWindow->PointModeEnabled());
    Point2D selectionPosition = scene->GetSelectionCenter(mainWindow->PointModeEnabled());

    mainWindow->UpdateSelectionPositionInput(selectionPosition.x, selectionPosition.y, hasSelection);

    //update a cursor
    mouseMoveEvent(event);

    repaint();
}

void Renderer::mouseMoveEvent(QMouseEvent *event)
{
    Point2D scenePoint = GetCursorPosition();

    if(scene->GetCreationMode() == CreationMode::SetLineEnd)
    {
        scene->SetCreateLinePosition(scenePoint);
        repaint();
    }

    bool leftButtonPressed = event && event->buttons() & Qt::LeftButton;
    bool rightButtonPressed = event && event->buttons() & Qt::RightButton;

    bool pointMode = mainWindow->PointModeEnabled();


    if((!leftButtonPressed && !rightButtonPressed) || !mouseHold)
    {
        if(scene->hasContentUnderCursor(scenePoint, pointMode))
            setCursor(Qt::CrossCursor);
        else
            setCursor(Qt::ArrowCursor);
        return;
    }

    QPoint velocity = QCursor::pos() - previousCursorPoint;

    float velX = velocity.x() * 2.0f / width();
    float velY = -velocity.y() * 2.0f / height();

    if(rightButtonPressed)
    {
        SetView(scene->viewX + velX / scene->screenScale,
                scene->viewY + velY / scene->screenScale);
    }
    else
        if(scene->GetCreationMode() == CreationMode::DontCreate)
        {
            bool shift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
            bool control = QApplication::keyboardModifiers() & Qt::ControlModifier;

            if(pointMode)
                scene->PointMode_Moved(scenePoint, mainWindow->GetSelectionMode(), shift);
            else
                scene->LineMode_Moved(scenePoint, mainWindow->GetSelectionMode(), shift, control);

        }

    previousCursorPoint = QCursor::pos();

    repaint();
}

void Renderer::wheelEvent(QWheelEvent *event)
{
    SetScale(scene->screenScale +
             event->delta() / 1000.0f * ((QApplication::keyboardModifiers() & Qt::ControlModifier) ? maxZoomAcceleration : minZoomAcceleration));
    mainWindow->UpdateViewScaleInput(scene->screenScale);
    repaint();
}

void Renderer::keyPressEvent(QKeyEvent *event)
{
    bool isMoveKey = false;

    switch (event->key())
    {
    case  Qt::Key_W:
        mainWindow->ToogleSelectionMoveButton();
        isMoveKey = true;
        break;
    case Qt::Key_E:
        mainWindow->ToogleSelectionRotateButton();
        isMoveKey = true;
        break;
    case Qt::Key_R:
        mainWindow->ToogleSelectionScaleButton();
        isMoveKey = true;
        break;
    case Qt::Key_P:
        if(!mouseHold)
            mainWindow->TooglePointModeButton();
        break;
    case Qt::Key_Delete:
        if(scene->GetCreationMode() != CreationMode::SetLineEnd)
        {
            scene->RemoveSelection(mainWindow->PointModeEnabled());
            scene->UnloadDataToRenderer(this);
            mouseReleaseEvent(0);
        }
        break;
    case Qt::Key_C:
        if(!mouseHold)
            mainWindow->EnableCreateLineMode();
        break;
    case Qt::Key_A:
        if(scene->GetCreationMode() == CreationMode::DontCreate &&
                (QApplication::keyboardModifiers() & Qt::ControlModifier))
        {
            scene->SelectAll();
            repaint();
        }
        break;
    case Qt::Key_Escape:
        mainWindow->UntoogleAllSelectionButtons();
        mainWindow->DisableCreateLineMode();
        break;
    default:
        return;
    }

    if(isMoveKey)
        scene->SaveSelectionState(GetCursorPosition(), mainWindow->PointModeEnabled());
    //qDebug() << "Key pressed" << event->key();
}


void Renderer::focusOutEvent(QFocusEvent *)
{
    mainWindow->DisableCreateLineMode();
}
