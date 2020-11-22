#include "scene.h"

Scene::Scene()
{
    viewX = 0;
    viewY = 0;
    screenScale = 1;
    lineWidth = 5;
}

void Scene::AddLine(const Line2D &line)
{
    lines.push_back(line);

    lineStartPositions.push_back(line);
    lineEndPositions.push_back(line);

    colors.push_back(Point2D());
    groups.push_back(0);
}

void Scene::MergeGroup(std::vector<Line2D> *lineGroup, QTreeWidgetItem *group)
{
    lines.insert(lines.end(), lineGroup->begin(), lineGroup->end());
    lineStartPositions.insert(lineStartPositions.end(), lineGroup->begin(), lineGroup->end());
    lineEndPositions.insert(lineEndPositions.end(), lineGroup->begin(), lineGroup->end());

    for(uint i = 0; i < lineGroup->size(); i++)
    {
        colors.push_back(Point2D());
        groups.push_back(group);
    }
}

void Scene::SelectPointByTouch(const Point2D &touch)
{
    ResetSelection();
    selectedPointIndex = GetPointAtPosition(touch);

    if(selectedPointIndex == -1)
        return;

    SetPointColor(selectedPointIndex, 1);

    //qDebug() << "selected Point index" << selectedPointIndex;

    int offset = selectedPointIndex % 2;
    selectedPointIndex = LineToTopZ(selectedPointIndex / 2) * 2 + offset;

    //qDebug() << "selected Point index after changes" << selectedPointIndex;
}

void Scene::SelectLineByTouch(const Point2D &touch, bool multiSelection, bool singleClick)
{
    int selectedLineIndex = GetLineAtPosition(touch);

    if(selectedLineIndex == -1)
    {
        if(!multiSelection)
            ResetSelection();
        return;
    }

    if(multiSelection)
    {
        if(groups[selectedLineIndex])
            ToogleGroupSelection(groups[selectedLineIndex]);
        else
            ToogleLineSelection(selectedLineIndex);
    }
    else
    {
        if(singleClick)
            for(uint i = 0; i < selectedLines.size(); i++)
            {
                if(selectedLines[i] == selectedLineIndex)
                    return;
            }

        bool lineSelected = selectedLines.size() == 1 && selectedLines[0] == selectedLineIndex;

        ResetSelection();

        if(groups[selectedLineIndex] && singleClick && !lineSelected)
            ToogleGroupSelection(groups[selectedLineIndex]);
        else
        {
            selectedLineIndex = LineToTopZ(selectedLineIndex);
            ToogleLineSelection(selectedLineIndex);
        }
    }

}

void Scene::PointMode_Moved(const Point2D &touch, SelectionMode mode, bool oneAxisMode)
{
    if(HasSelection(true) && mode == SelectionMode::SELECTION_MOVE)
        TransferPoint(touch, oneAxisMode);

}

void Scene::LineMode_Moved(const Point2D &touch, SelectionMode mode, bool oneAxisMode, bool proportional)
{
    if(!HasSelection(false))
        return;

    switch (mode) {
    case SelectionMode::SELECTION_MOVE:
        TransferSelection(touch, oneAxisMode);
        break;
    case SelectionMode::SELECTION_ROTATE:
        RotateSelection(touch);
        break;
    case SelectionMode::SELECTION_SCALE:
        ScaleSelection(touch, oneAxisMode, proportional);
        break;
    default:
        break;
    }
}

void Scene::RemoveSelection(bool pointMode)
{
    if(!HasSelection(pointMode))
        return;

    if(pointMode)
    {
        int pointIndex = selectedPointIndex;

        ResetSelection();

        lines.erase(lines.begin() + pointIndex / 2);

        lineStartPositions.erase(lineStartPositions.begin() + pointIndex / 2);
        lineEndPositions.erase(lineEndPositions.begin() + pointIndex / 2);

        groups.erase(groups.begin() + pointIndex / 2);
        colors.erase(colors.begin() + pointIndex / 2);
    }
    else
    {
        auto selectedLines = this->selectedLines;

        ResetSelection();

        std::sort(selectedLines.begin(), selectedLines.end(),
                  [](int a, int b) -> bool
        {
            return a > b;
        });

        for(uint i = 0; i < selectedLines.size(); i++)
        {
            int lineIndex = selectedLines[i];

            lines.erase(lines.begin() + lineIndex);

            lineStartPositions.erase(lineStartPositions.begin() + lineIndex);
            lineEndPositions.erase(lineEndPositions.begin() + lineIndex);

            groups.erase(groups.begin() + lineIndex);
            colors.erase(colors.begin() + lineIndex);
        }
    }
}

void Scene::ToogleGroupSelection(QTreeWidgetItem *group, bool headSelect)
{
    for(int i = 0; i < group->childCount(); i++)
        ToogleGroupSelection(group->child(i), false);

    for(uint i = 0; i < lines.size(); i++)
    {
        if(groups[i] == group)
            ToogleLineSelection(i, headSelect);
    }
}

void Scene::SetSelectionGroup(QTreeWidgetItem *group)
{
    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines[i];
        groups[lineIndex] = group;
    }

    if(selectedPointIndex != -1)
        groups[selectedPointIndex / 2] = group;

}

void Scene::SelectAll()
{
    ResetSelection();
    for(uint i = 0; i < lines.size(); i++)
        ToogleLineSelection(i);
}

void Scene::ResetSelection()
{
    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines[i];
        SetLineColor(lineIndex, 0);
    }

    if(HasSelection(true))
        SetPointColor(selectedPointIndex, 0);

    selectedLines.clear();
    originalLineShapes.clear();

    selectedPointIndex = -1;
}

void Scene::ResetGroup(QTreeWidgetItem *removedGroup)
{
    for(uint i = 0; i < groups.size(); i++)
        if(groups[i] == removedGroup)
            groups[i] = 0;
}

void Scene::SaveGroups(QTreeWidgetItem *item)
{
    savedItem = item;
}

QTreeWidgetItem *Scene::LoadGroups() const
{
    return savedItem;
}

bool Scene::HasSelection(bool pointMode) const
{
    return pointMode ? selectedPointIndex != -1 : selectedLines.size() != 0;
}

//bool Scene::IsLineSelected() const
//{
//    return selectedLinesIndexes.size() != 0;
//}

Point2D Scene::GetSelectionCenter(bool pointMode) const
{
    Point2D selectionCenter;

    if(!HasSelection(pointMode))
        return selectionCenter;

    if(pointMode)
    {
        if(selectedPointIndex % 2)
            return lines[selectedPointIndex / 2].b;
        else
            return lines[selectedPointIndex / 2].a;
    }

    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines [i];
        Line2D line = lines[lineIndex];

        selectionCenter +=  (line.a + line.b);
    }

    return selectionCenter / (selectedLines.size() * 2);
}

void Scene::UnloadDataToRenderer(Renderer *renderer)
{
    renderer->LoadLines(lines);
    renderer->LoadColors(colors);
}

void Scene::SetMorphingStart()
{
    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines[i];
        lineStartPositions[lineIndex] = lines[lineIndex];
    }
}

void Scene::SetMorphingEnd()
{
    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines[i];
        lineEndPositions[lineIndex] = lines[lineIndex];
    }
}

void Scene::ResetMorphing()
{
    SetMorphingStart();
    SetMorphingEnd();
}

void Scene::SetMorphingState(float morphing)
{
    for(uint i = 0; i < lines.size(); i++)
    {
        Point2D a = lineStartPositions[i].a +  (lineEndPositions[i].a - lineStartPositions[i].a) * morphing;
        Point2D b = lineStartPositions[i].b +  (lineEndPositions[i].b - lineStartPositions[i].b) * morphing;

        lines[i] = Line2D(a, b);
    }
}

void Scene::SetSelectionPosition(const Point2D &position, bool pointMode)
{
    Point2D offset = position - GetSelectionCenter(pointMode);

    QMatrix4x4 matrix;

    matrix.translate(offset.x, offset.y);

    ApplyMatrix(matrix, pointMode);
}

void Scene::SetLineColor(int lineIndex, float color)
{
    colors[lineIndex] = Point2D(color, color);
}

void Scene::SetPointColor(int pointIndex, int color)
{
    Point2D lineColor = colors[pointIndex / 2];

    if(pointIndex % 2)
        lineColor.y = color;
    else
        lineColor.x = color;

    colors[pointIndex / 2] = lineColor;
}

//bool Scene::IsLineInSelection(int lineIndex) const
//{
//    for(int i = 0; i < selectedLines.size(); i++)
//    {
//        if(selectedLines[i] == lineIndex)
//            return true;
//    }
//    return false;
//}

void Scene::ToogleLineSelection(int lineIndex, bool isHeadSelection)
{
    for(uint i = 0; i < selectedLines.size(); i++)
        if(selectedLines[i] == lineIndex)
        {
            selectedLines.erase(selectedLines.begin() + i);
            originalLineShapes.erase(originalLineShapes.begin() + i);
            SetLineColor(lineIndex, 0);
            return;
        }

    selectedLines.push_back(lineIndex);
    SetLineColor(lineIndex, isHeadSelection ? 1 : 0.5f);
}

void Scene::TransferPoint(const Point2D &touch, bool oneAxisMode)
{
    Point2D offset = touch - startTouch;

    Point2D pointPos = touch;

    if(oneAxisMode)
    {
        if(float_abs(offset.x) > float_abs(offset.y))
            pointPos.y = startTouch.y;
        else
            pointPos.x = startTouch.x;
    }

    if(selectedPointIndex % 2)
        lines[selectedPointIndex / 2].b = pointPos;
    else
        lines[selectedPointIndex / 2].a = pointPos;

}

void Scene::TransferSelection(const Point2D &touch, bool oneAxisMode)
{
    Point2D offset = touch - startTouch;

    if(oneAxisMode)
    {
        if(float_abs(offset.x) > float_abs(offset.y))
            offset.y = 0;
        else
            offset.x = 0;
    }

    for(uint i = 0; i < selectedLines.size(); i++)
        lines[selectedLines[i]] = Line2D::Transfer(originalLineShapes[i], offset);
}

void Scene::RotateSelection(const Point2D &touch)
{
    Point2D originalVector = startTouch - selectionCenter;
    Point2D newVector = touch - selectionCenter;

    float x1 = originalVector.x;
    float y1 = originalVector.y;

    float x2 = newVector.x;
    float y2 = newVector.y;

    float dot = x1*x2 + y1 * y2;//      # dot product between [x1, y1] and [x2, y2]
    float det = x1*y2 - y1*x2;//      # determinant
    float angle = atan2(det, dot);

    for(uint i = 0; i < selectedLines.size(); i++)
        lines[selectedLines[i]] = Line2D::Rotate(originalLineShapes[i], selectionCenter, angle);
}

void Scene::ScaleSelection(const Point2D &touch, bool oneAxisMode, bool proportional)
{
    Point2D offset = touch - startTouch;

    if(proportional)
        offset.y = offset.x;
    else
        if(oneAxisMode)
        {
            if(float_abs(offset.x) > float_abs(offset.y))
                offset.y = 0;
            else
                offset.x = 0;
        }

    for(uint i = 0; i < selectedLines.size(); i++)
        lines[selectedLines[i]] = Line2D::Scale(originalLineShapes[i], selectionCenter, offset);
}

void Scene::ApplyMatrix(const QMatrix4x4 &matrix, bool pointMode)
{
    if(!HasSelection(pointMode))
        return;

    if(pointMode)
    {
        if(selectedPointIndex % 2)
            lines[selectedPointIndex / 2].b *= matrix;
        else
            lines[selectedPointIndex / 2].a *= matrix;
    }
    else
    {
        for(uint i = 0; i < selectedLines.size(); i++)
        {
            int lineIndex = selectedLines[i];
            lines[lineIndex].a *= matrix;
            lines[lineIndex].b *= matrix;
        }
    }
}

Point2D Scene::IntersectPoint(const Point2D &touch, const Point2D &a, const Point2D &b)
{
    if(float_abs(b.y - a.y) < 0.001f) //if(a.y = b.y)
        return Point2D(touch.x, a.y);

    if(float_abs(b.x - a.x) < 0.001f) //if(a.x == b.x)
        return Point2D(a.x, touch.y);

    float k1 = (b.y - a.y) / (b.x - a.x);
    float k2 = -1.0f / k1;

    float b1 = a.y - k1 * a.x;
    float b2 = touch.y - k2 * touch.x;

    Point2D intersection;

    intersection.x = (b2 - b1) / (k1 - k2);
    intersection.y = k1 * intersection.x + b1;

    return intersection;
}

int Scene::GetLineAtPosition(const Point2D &touch) const
{
    int selectedLineIndex = -1;
    float offset = lineWidth / screenScale / screenScale / 2;

    for(uint i = 0; i < lines.size(); i++)
    {
        Line2D current = lines[i];

        Point2D intersection = IntersectPoint(touch, current.a, current.b);

        //20000 is magic const used for compensate observational error
        float maxX = float_max(current.a.x, current.b.x) * 20000 + offset;
        float minX = float_min(current.a.x, current.b.x) * 20000 - offset;

        float maxY = float_max(current.a.y, current.b.y) * 20000 + offset;
        float minY = float_min(current.a.y, current.b.y) * 20000 - offset;

        float length =  (intersection - touch).sqrLength();
        // if missclick
        if(
                intersection.x * 20000 < minX ||
                intersection.x * 20000 > maxX ||
                intersection.y * 20000 < minY ||
                intersection.y * 20000 > maxY ||
                length * 20000 > offset)
            continue;

        selectedLineIndex = i;
    }

    return selectedLineIndex;
}

int Scene::GetPointAtPosition(const Point2D &touch) const
{
    int selectedPointIndex = -1;

    float offset = lineWidth / screenScale / screenScale;

    for(uint i = 0; i < lines.size(); i++)
    {
        Line2D currentLine = lines[i];

        if((currentLine.a - touch).sqrLength() * 2000 < offset)
            selectedPointIndex = i * 2;
        if((currentLine.b - touch).sqrLength() * 2000 < offset)
            selectedPointIndex = i * 2 + 1;
    }

    return selectedPointIndex;
}

int Scene::LineToTopZ(int lineIndex)
{
    auto linesIterator = lines.begin() + lineIndex;
    auto groupsIterator = groups.begin() + lineIndex;
    auto colorsIterator = colors.begin() + lineIndex;

    auto lineStartIterator = lineStartPositions.begin() + lineIndex;
    auto lineEndIterator = lineEndPositions.begin() + lineIndex;

    std::rotate(linesIterator, linesIterator + 1, lines.end());
    std::rotate(groupsIterator, groupsIterator + 1, groups.end());
    std::rotate(colorsIterator, colorsIterator + 1, colors.end());

    std::rotate(lineStartIterator, lineStartIterator + 1, lineStartPositions.end());
    std::rotate(lineEndIterator, lineEndIterator + 1, lineEndPositions.end());

    return lines.size() - 1;
}

//void Scene::SetSelectionModifyStartupPosition(const Point2D &touch, bool pointMode)
//{
//        startTouch = touch;

//    if((pointMode && (selectedPointIndex == -1)) || (!pointMode && (selectedLineIndex == -1)))
//        return;

//    int selectedIndex = pointMode ? selectedPointIndex / 2 : selectedLineIndex;

//    startA = lines[selectedIndex].a;
//    startB = lines[selectedIndex].b;
//}

bool Scene::hasContentUnderCursor(const Point2D &touch, bool pointMode) const
{
    return (pointMode ? GetPointAtPosition(touch) : GetLineAtPosition(touch)) != -1;
}

QJsonArray Scene::SaveToJson() const
{
    QJsonArray linesArray;

    for(uint i = 0; i < lines.size(); i++)
    {
        Line2D currentLine = lines[i];

        QJsonArray lineData;

        lineData.append(QJsonValue(currentLine.a.x));
        lineData.append(QJsonValue(currentLine.a.y));
        lineData.append(QJsonValue(currentLine.b.x));
        lineData.append(QJsonValue(currentLine.b.y));

        lineData.append(QJsonValue(groups[i] ? groups[i]->text(0) : ""));

        linesArray.append(QJsonValue(lineData));
    }
    return linesArray;
}

QTreeWidgetItem *Scene::GetGroup(QTreeWidgetItem *current, const QString &groupName) const
{
    QTreeWidgetItem *item = 0;

    if(current->text(0) == groupName)
        return current;

    for(int i = 0; i < current->childCount(); i++)
    {
        item = GetGroup(current->child(i), groupName);
        if(item != 0)
            return item;
    }

    return 0;

}

bool Scene::LoadFromJson(const QJsonArray &lineData, QTreeWidget *groupsTree)
{
    for(int i = 0; i < lineData.count(); i++)
    {
        QJsonArray line = lineData.at(i).toArray();

        if(line.size() != 5)
            return false;

        AddLine(Line2D(line.at(0).toDouble(), line.at(1).toDouble(), line.at(2).toDouble(), line.at(3).toDouble()));

        QString groupName = line.at(4).toString();

        if(groupName.isEmpty())
            continue;

        int currentLineIndex = groups.size() - 1;

        for(int k = 0; k < groupsTree->topLevelItemCount(); k++)
        {
            auto item = GetGroup(groupsTree->topLevelItem(k), groupName);

            if(item != 0)
            {
                groups[currentLineIndex] = item;
                break;
            }
        }
    }

    return true;
}

void Scene::ClearScene()
{
    ResetSelection();

    lines.clear();
    lineStartPositions.clear();
    lineEndPositions.clear();

    colors.clear();
    groups.clear();
}

void Scene::SaveSelectionState(const Point2D &touch, bool pointMode)
{
    originalLineShapes.clear();

    for(uint i = 0; i < selectedLines.size(); i++)
    {
        int lineIndex = selectedLines[i];
        originalLineShapes.push_back(lines[lineIndex]);
    }

    selectionCenter = GetSelectionCenter(pointMode);
    startTouch = touch;
}

void Scene::EnableCreateLine()
{
    ResetSelection();
    _lineCreationMode = CreationMode::SetLineStart;
}

void Scene::SetCreateLineStart(const Point2D &lineStart)
{
    specialLine = Line2D(lineStart, lineStart);
    _lineCreationMode = CreationMode::SetLineEnd;
}

void Scene::SetCreateLineEnd(const Point2D &lineEnd, bool createNextLine)
{
    SetCreateLinePosition(lineEnd);

    AddLine(specialLine);

    ToogleLineSelection(lines.size() - 1);

    _lineCreationMode = CreationMode::SetLineStart;

    if(createNextLine)
        SetCreateLineStart(lineEnd);
}

void Scene::SetCreateLinePosition(const Point2D &pointPosition)
{
    specialLine.b = pointPosition;
}

void Scene::DisableCreateLine()
{
    _lineCreationMode = CreationMode::DontCreate;
}

CreationMode Scene::GetCreationMode() const
{
    return _lineCreationMode;
}

float *Scene::GetSpecialLineData() const
{
    return (float*)(&specialLine);
}
