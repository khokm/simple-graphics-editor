#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "line2d.h"
#include "renderer.h"
#include "float_math.h"
#include <QTreeWidgetItem>
#include <QJsonArray>

class Scene
{
public:
    float screenScale, viewX, viewY, lineWidth;

    Scene();
    void AddLine(const Line2D& line);

    void MergeGroup(std::vector<Line2D> *lineGroup, QTreeWidgetItem *group);

    void RemoveLine(int index);

    void SelectPointByTouch(const Point2D& touch);
    void SelectLineByTouch(const Point2D &touch, bool multiSelection, bool singleClick);

    void PointMode_Moved(const Point2D& touch, SelectionMode mode, bool oneAxisMode);
    void LineMode_Moved(const Point2D& touch, SelectionMode mode, bool oneAxisMode, bool proportional);

    void RemoveSelection(bool pointMode);

    void ToogleGroupSelection(QTreeWidgetItem *group, bool headSelect = true);
    void SetSelectionGroup(QTreeWidgetItem *group);

    void SelectAll();
    void ResetSelection();
    void ResetGroup(QTreeWidgetItem *removedGroup);

    void SaveGroups(QTreeWidgetItem *item);
    QTreeWidgetItem *LoadGroups() const;

    bool HasSelection(bool pointMode) const;
    Point2D GetSelectionCenter(bool pointMode) const;

    void ApplyMatrix(const QMatrix4x4 &matrix, bool pointMode);

    void MouseReleased();
    void UnloadDataToRenderer(class Renderer *renderer);

    void SetMorphingStart();

    void SetMorphingEnd();

    void ResetMorphing();

    void SetMorphingState(float morphing);

    void SetSelectionPosition(const Point2D &position, bool pointMode);

    bool hasContentUnderCursor(const Point2D &touch, bool pointMode) const;

    QJsonArray SaveToJson() const;

    QTreeWidgetItem *GetGroup(QTreeWidgetItem *current, const QString &groupName) const;

    bool LoadFromJson(const QJsonArray &lineData, QTreeWidget *groupsTree);

    void ClearScene();

    void SaveSelectionState(const Point2D &touch, bool pointMode);

    void EnableCreateLine();

    void SetCreateLineStart(const Point2D &lineStart);
    void SetCreateLineEnd(const Point2D &lineEnd, bool createNextLine);
    void SetCreateLinePosition(const Point2D &pointPosition);
    void DisableCreateLine();

    CreationMode GetCreationMode() const;

    float *GetSpecialLineData() const;

private:
    Line2D specialLine;
    CreationMode _lineCreationMode = CreationMode::DontCreate;

    QTreeWidgetItem *savedItem = 0;

    int selectedPointIndex = -1;

    std::vector<int> selectedLines;
    std::vector<Line2D> originalLineShapes;

    std::vector<Line2D> lines;
    std::vector<Line2D> lineStartPositions;
    std::vector<Line2D> lineEndPositions;

    std::vector<Point2D> colors;
    std::vector<QTreeWidgetItem*> groups;

    Point2D startTouch, selectionCenter;

    void SetLineColor(int lineIndex, float color);
    void SetPointColor(int pointIndex, int color);

    //bool IsLineInSelection(int lineIndex) const;
    void ToogleLineSelection(int lineIndex, bool isHeadSelection = true);

    void TransferPoint(const Point2D &touch, bool oneAxisMode);

    void TransferSelection(const Point2D &touch, bool oneAxisMode);
    void RotateSelection(const Point2D &touch);
    void ScaleSelection(const Point2D &touch, bool oneAxisMode, bool proportional);


    static Point2D IntersectPoint(const Point2D &touch, const Point2D &a, const Point2D &b);

    int GetLineAtPosition(const Point2D &touch) const;
    int GetPointAtPosition(const Point2D &touch) const;

    int LineToTopZ(int lineIndex);
};

#endif // SCENE_H
