#include "fractalglwidget.h"

FractalGLWidget::FractalGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

FractalGLWidget::~FractalGLWidget()
{

}

void FractalGLWidget::BuildTree(int deep, int branchCount, bool firstRight, float branchAngle, int &totalCount)
{
    lines.clear();
    colors.clear();

    minX = 1;
    maxX = -1;
    minY = 1;
    maxY = -1;

    const double toRad = M_PI / 180;
    CreateBranch(Point2D(0, 0), Point2D(0, 1), 1, deep, branchCount, firstRight, branchAngle * toRad, totalCount);
    repaint();
}

void FractalGLWidget::CreateBranch(const Point2D &start, const Point2D &end, int currentDeep, int maxDeep, int branchCount, bool rightBranch, float branchAngle, int &totalCount)
{
    //push current branch
    Line2D currentBranch = Line2D(start, end);

    lines.push_back(currentBranch);
    colors.push_back(defaultLineColor);
    totalCount++;

    minX = float_min(minX, float_min(start.x, end.x));
    maxX = float_max(maxX, float_max(start.x, end.x));
    minY = float_min(minY, float_min(start.y, end.y));
    maxY = float_max(maxY, float_max(start.y, end.y));

    if(currentDeep >= maxDeep)
        return;

    //create child branches

    float childBranchLength = currentBranch.Length() * (1.0f - (float)currentDeep / maxDeep);

    Point2D branchVector = (end - start).normalized();

    Point2D rotatedLeftVector = Point2D::RotatePoint(branchVector, Point2D(0, 0), branchAngle) * childBranchLength;

    Point2D rotatedRightVector = Point2D::RotatePoint(branchVector, Point2D(0, 0), -branchAngle) * childBranchLength;

    for(int i = 0; i < branchCount; i++)
    {
       // if(RandomFloat(0, 1.0f - branchProbability) >= 0.01f)
         //   return;

        Point2D childBranchStart = start + (end - start) * ((float)(i + 1) / (branchCount + 1));

        Point2D childBranchEnd = (rightBranch ? rotatedRightVector : rotatedLeftVector) + childBranchStart;

        CreateBranch(childBranchStart, childBranchEnd, currentDeep + 1, maxDeep, branchCount, rightBranch, branchAngle, totalCount);
        rightBranch = !rightBranch;

    }
}

void FractalGLWidget::initializeGL()
{
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":/Shaders/vShader.glsl");

    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":/Shaders/fShader.glsl");

    m_program.addShader(&vShader);
    m_program.addShader(&fShader);

    if(!m_program.link())
    {
        qWarning("Fractal Shader link error");
        exit(0);
    }

    m_matrixUniform = m_program.uniformLocation("matrix2D");
    m_vertAttrib = m_program.attributeLocation("vert2D");
    m_colorAttrib = m_program.attributeLocation("vertexColor");

    m_program.enableAttributeArray(m_vertAttrib);
    m_program.enableAttributeArray(m_colorAttrib);

    glClearColor(1, 1, 1, 1);

    glLineWidth(2);

    if(!m_program.bind())
    {
        qWarning("fractal shader program error");
        exit(0);
    }

}

void FractalGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void FractalGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if(lines.size() == 0)
        return;

    QMatrix4x4 projectionMatrix, viewMatrix;

    viewMatrix.translate((minX + maxX) / 2, -(minY + maxY) /2 );

    //float offset = float_max((maxX - minX) / 2, (maxY - minY) / 2);
    float scale = float_max(maxX - minX, maxY - minY);

    float aspectX = (height() < width() ? (float)width() / height() : 1) * scale * 0.5f;// + offset;
    float aspectY = (height() > width() ? (float)height() / width() : 1) * scale * 0.5f;// + offset;



    //projectionMatrix.ortho(minX, maxX, minY, maxY, 1, -1);
    projectionMatrix.ortho(-aspectX, aspectX, -aspectY, aspectY, 1, -1);

//    projectionMatrix.ortho(minX -0.1f, maxX + 0.1f, minY -0.1f, maxY + 0.1f, 1, -1);

    m_program.setUniformValue(m_matrixUniform, projectionMatrix * viewMatrix);
    m_program.setAttributeArray(m_vertAttrib, (float*)&lines[0], 2);
    m_program.setAttributeArray(m_colorAttrib, (float*)&colors[0], 1);

    glDrawArrays(GL_LINES, 0, lines.size() * 2);
}
