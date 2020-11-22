#include "mainwindow.h"
#include "ui_mainwindow.h"

QByteArray arr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->addWidget(ui->sceneList_ComboBox);
    renderer = ui->renderer;

    //create default scene
    on_addScene_Action_triggered();
}

MainWindow::~MainWindow()
{
    if(enterMatrixDialog)
        enterMatrixDialog->deleteLater();

    delete ui;
}

void MainWindow::UpdateViewPositionInput(float viewX, float viewY)
{
    ui->viewX_DoubleSpinBox->setValue(-viewX);
    ui->viewY_DoubleSpinBox->setValue(-viewY);
}

void MainWindow::UpdateViewScaleInput(float scale)
{
    ui->viewScale_SpinBox->setValue((int)(scale * 100));
}

void MainWindow::UpdateSelectionPositionInput(float selectionX, float selectionY, bool enabled)
{
    ui->selectionX_DoubleSpinBox->setValue(selectionX);
    ui->selectionY_DoubleSpinBox->setValue(selectionY);
    ui->selectionX_DoubleSpinBox->setEnabled(enabled);
    ui->selectionY_DoubleSpinBox->setEnabled(enabled);
}

void MainWindow::ToogleSelectionMoveButton()
{
    ui->selectionMove_Action->toggle();
}

void MainWindow::ToogleSelectionRotateButton()
{
    ui->selectionRotate_Action->toggle();
}

void MainWindow::ToogleSelectionScaleButton()
{
    ui->selectionScale_Action->toggle();
}

void MainWindow::TooglePointModeButton()
{
    if(ui->pointMode_Action->isVisible())
        ui->pointMode_Action->trigger();
    else
        ui->lineMode_Action->trigger();
}

void MainWindow::UntoogleAllSelectionButtons()
{
    ui->selectionMove_Action->toggle();
    ui->selectionMove_Action->setChecked(false);
}

SelectionMode MainWindow::GetSelectionMode() const
{
    if(ui->selectionMove_Action->isChecked())
        return SelectionMode::SELECTION_MOVE;
    if(ui->selectionRotate_Action->isChecked())
        return SelectionMode::SELECTION_ROTATE;
    if(ui->selectionScale_Action->isChecked())
        return SelectionMode::SELECTION_SCALE;

    return SelectionMode::FREE;
}

bool MainWindow::PointModeEnabled() const
{
    return ui->pointMode_Action->isVisible();
}

void MainWindow::EnableCreateLineMode()
{
    ui->statusBar->showMessage("Включен режим создания линий, нажмите ESCAPE для отмены");
    renderer->GetScene()->EnableCreateLine();

    ui->createCustomLine_PushButton->setEnabled(false);
    ui->groupSelect_PushButton->setEnabled(false);

    renderer->repaint();
}

void MainWindow::DisableCreateLineMode()
{
    ui->statusBar->clearMessage();
    renderer->GetScene()->DisableCreateLine();

    ui->createCustomLine_PushButton->setEnabled(true);
    ui->groupSelect_PushButton->setEnabled(ui->groupsTree_TreeWidget->currentItem() != 0);

    renderer->repaint();
}

void MainWindow::on_selectionMove_Action_toggled(bool arg1)
{
    if(!arg1)
        return;
    ui->selectionRotate_Action->setChecked(false);
    ui->selectionScale_Action->setChecked(false);
}

void MainWindow::on_selectionRotate_Action_toggled(bool arg1)
{
    if(!arg1)
        return;
    ui->selectionMove_Action->setChecked(false);
    ui->selectionScale_Action->setChecked(false);
}

void MainWindow::on_selectionScale_Action_toggled(bool arg1)
{
    if(!arg1)
        return;
    ui->selectionMove_Action->setChecked(false);
    ui->selectionRotate_Action->setChecked(false);
}

void MainWindow::OnSelectionCoords_EditingFinished()
{

    Point2D point(ui->selectionX_DoubleSpinBox->value(), ui->selectionY_DoubleSpinBox->value());
    renderer->GetScene()->SetSelectionPosition(point, PointModeEnabled());

    renderer->repaint();
}

void MainWindow::OnViewCoords_EditingFinished()
{
    renderer->SetView(-ui->viewX_DoubleSpinBox->value(), -ui->viewY_DoubleSpinBox->value());
    renderer->repaint();
}

void MainWindow::OnViewScale_EditingFinished()
{
    renderer->SetScale((float)(ui->viewScale_SpinBox->value()) / 100);
    renderer->repaint();
}

void MainWindow::OnCurrentScene_IndexChanged(int)
{
    QTreeWidgetItem *savedCopy = new QTreeWidgetItem();

    while(ui->groupsTree_TreeWidget->topLevelItemCount() != 0)
    {
        QTreeWidgetItem *item = ui->groupsTree_TreeWidget->takeTopLevelItem(0);
        savedCopy->addChild(item);
    }

    renderer->GetScene()->SaveGroups(savedCopy);

    Scene* currentScene = (Scene*)(ui->sceneList_ComboBox->currentData().value<void*>());

    savedCopy = currentScene->LoadGroups();

    if(savedCopy != 0)
    {
        //        qDebug() << "it has " << savedCopy->childCount() << "childs!";

        while(savedCopy->childCount() != 0)
        {
            ui->groupsTree_TreeWidget->addTopLevelItem(savedCopy->takeChild(0));
        }
    }

    DisableCreateLineMode();
    renderer->SetScene(currentScene);
    renderer->setFocus();
}

void MainWindow::on_addScene_Action_triggered()
{
    int sceneNumber = 1;

    QString name;

    while(true)
    {
        name = QString("Новая сцена %1").arg(sceneNumber);
        bool nameExist = false;

        for(int k = 0; k < ui->sceneList_ComboBox->count(); k++)
        {
            if(ui->sceneList_ComboBox->itemText(k) == name)
            {
                nameExist = true;
                break;
            }
        }
        if(!nameExist)
            break;

        sceneNumber += 1;

    }

    Scene *newScene = new Scene();

    //newScene->AddLine(Line2D(0, 0, 1, 0));
    //newScene->AddLine(Line2D(0, 0, 0, 1));

    ui->sceneList_ComboBox->addItem(name, QVariant::fromValue((void*)newScene));

    if(ui->sceneList_ComboBox->count() > 1)
        ui->removeScene_Action->setEnabled(true);

    ui->sceneList_ComboBox->setCurrentIndex(ui->sceneList_ComboBox->count() - 1);
}

void MainWindow::UpdateBottomPanelPosition()
{
    if(!renderer)
        return;

    QBoxLayout *layout = renderer->width() < 512 ?
                (QBoxLayout*)ui->bottomPanel_VerticalGridLayout :
                (QBoxLayout*)ui->bottomPanel_HorizontalGridLayout;

    layout->addWidget(ui->selectionCoords_GroupBox);
    layout->addWidget(ui->viewCoords_GroupBox);
}

void MainWindow::UpdateMorphingPanel()
{
    bool small = ui->animation_DockWidget->width() < 640;

    QBoxLayout *layout = small ?
                (QBoxLayout*)ui->anim_HorizGridLayout:
                (QBoxLayout*)ui->anim_VertGridLayout;

    layout->addWidget(ui->morphingSetStartPoint_PushButton);
    layout->addWidget(ui->morphingSetEndPoint_PushButton);
    layout->addWidget(ui->morphingReset_PushButton);

    ui->animation_DockWidget->setMaximumHeight(small ? 200 : 100);
    ui->animation_DockWidget->setMinimumHeight(small ? 200 : 100);
    ui->animation_DockWidget->setMinimumWidth(100);
}

void MainWindow::on_removeScene_Action_triggered()
{
    int sceneIndex = ui->sceneList_ComboBox->currentIndex();

    Scene* currentScene = (Scene*)(ui->sceneList_ComboBox->currentData().value<void*>());
    ui->sceneList_ComboBox->removeItem(sceneIndex);

    if(ui->sceneList_ComboBox->count() < 2)
    {
        ui->removeScene_Action->setEnabled(false);
    }

    delete currentScene;
}

void MainWindow::on_renameScene_Action_triggered()
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("Переимнование сцены"),
                                         tr("Введите новое название сцены:"), QLineEdit::Normal,
                                         ui->sceneList_ComboBox->currentText(), &ok);

    if(ok && !text.isEmpty())
    {

        for(int k = 0; k < ui->sceneList_ComboBox->count(); k++)
        {
            if(ui->sceneList_ComboBox->itemText(k) == text)
            {

                QMessageBox::critical(this, tr("Переименование сцены"),
                                      tr("Сцена с таким именем уже существует!"),
                                      QMessageBox::Ok);
                on_renameScene_Action_triggered();
                return;
            }

        }

        ui->sceneList_ComboBox->setItemText(ui->sceneList_ComboBox->currentIndex(), text);
    }
}

void MainWindow::on_setLineWidth_Action_triggered()
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("Установить ширину линий"),
                                         tr("Введите ширину линий:"), QLineEdit::Normal,
                                         QString::number(renderer->GetLineWidth()), &ok);

    if(ok && !text.isEmpty())
    {
        bool convert;

        float value = text.toFloat(&convert);

        if(!convert){
            QMessageBox::critical(this, tr("Установить ширину линий"),
                                  tr("Некорректное значение!"),
                                  QMessageBox::Ok);

            on_setLineWidth_Action_triggered();
        }
        else
        {
            if(value < 1 || value > 20)
            {
                QMessageBox::critical(this, tr("Установить ширину линий"),
                                      tr("Значение должно быть в пределах от 1 до 20!"),
                                      QMessageBox::Ok);
                on_setLineWidth_Action_triggered();

            }
            else
            {
                renderer->SetLineWidth(value);
                renderer->repaint();
            }
        }

    }

}

void MainWindow::on_createCustomLine_PushButton_clicked()
{
    float lineStartX = ui->createLineAX_DoubleSpinBox->value();
    float lineStartY = ui->createLineAY_DoubleSpinBox->value();
    float lineEndX = ui->createLineBX_DoubleSpinBox->value();
    float lineEndY = ui->createLineBY_DoubleSpinBox->value();

    if(ui->randomLine_CheckBox->isChecked())
    {
        lineStartX = RandomFloat(lineStartX, lineEndX);
        lineStartY = RandomFloat(lineStartY, lineEndY);
        lineEndX = RandomFloat(lineStartX, lineEndX);
        lineEndY = RandomFloat(lineStartY, lineEndY);
    }

    Scene *scene = renderer->GetScene();

    scene->AddLine(Line2D(lineStartX, lineStartY, lineEndX, lineEndY));

    scene->UnloadDataToRenderer(renderer);

    renderer->repaint();
}

void MainWindow::on_lineMode_Action_triggered()
{
    ui->lineMode_Action->setVisible(false);
    ui->pointMode_Action->setVisible(true);

    ui->selectionMove_Action->toggle();
    ui->selectionMove_Action->toggle();

    ui->selectionRotate_Action->setCheckable(false);
    ui->selectionScale_Action->setCheckable(false);

    ui->selectionRotate_Action->setEnabled(false);
    ui->selectionScale_Action->setEnabled(false);


    renderer->GetScene()->ResetSelection();
    renderer->repaint();
}

void MainWindow::on_pointMode_Action_triggered()
{
    ui->lineMode_Action->setVisible(true);
    ui->pointMode_Action->setVisible(false);

    ui->selectionRotate_Action->setCheckable(true);
    ui->selectionScale_Action->setCheckable(true);

    ui->selectionRotate_Action->setEnabled(true);
    ui->selectionScale_Action->setEnabled(true);

    renderer->GetScene()->ResetSelection();
    renderer->repaint();
}


//GROUPS

//User select new element
void MainWindow::on_groupsTree_TreeWidget_itemSelectionChanged()
{
    bool notEmpty = ui->groupsTree_TreeWidget->currentItem() != 0;

    ui->groupRemove_PushButton->setEnabled(notEmpty);
    ui->groupSelect_PushButton->setEnabled(notEmpty && renderer->GetScene()->GetCreationMode() == CreationMode::DontCreate);
    ui->groupMerge_PushButton->setEnabled(notEmpty);
    ui->groupUngroup_PushButton->setEnabled(notEmpty);

    if(notEmpty)
        groupsTree_prevItemText = ui->groupsTree_TreeWidget->currentItem()->text(0);
}

//On item renamed
void MainWindow::on_groupsTree_TreeWidget_itemChanged(QTreeWidgetItem *item, int)
{
    QString newItemText = item->text(0);

    ui->groupsTree_TreeWidget->blockSignals(true);
    item->setText(0, groupsTree_prevItemText);
    ui->groupsTree_TreeWidget->blockSignals(false);

    if(newItemText.isEmpty())
        return;

    for(int i = 0; i < ui->groupsTree_TreeWidget->topLevelItemCount(); i++)
    {
        if(GroupExist(ui->groupsTree_TreeWidget->topLevelItem(i), newItemText))
            return;
    }

    ui->groupsTree_TreeWidget->blockSignals(true);
    item->setText(0, newItemText);
    ui->groupsTree_TreeWidget->blockSignals(false);

    groupsTree_prevItemText = newItemText;
}

QTreeWidgetItem *MainWindow::CreateGroup(const QString &name) const
{
    QString defaultGroupText = name;

    int iterator = 1;

    while(true)
    {
        bool exist = false;
        for(int i = 0; i < ui->groupsTree_TreeWidget->topLevelItemCount(); i++)
        {
            if(GroupExist(ui->groupsTree_TreeWidget->topLevelItem(i), defaultGroupText))
            {
                exist = true;
                break;
            }
        }

        if(!exist)
            break;

        defaultGroupText = QString("%1 (%2)").arg(name, QString::number(iterator++));
    }

    QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();

    treeWidgetItem->setText(0, defaultGroupText);
    treeWidgetItem->setFlags(treeWidgetItem->flags() | Qt::ItemIsEditable);

    return treeWidgetItem;
}

bool MainWindow::GroupExist(QTreeWidgetItem *group, const QString &groupName) const
{
    for(int i = 0; i < group->childCount(); i++)
    {
        if(GroupExist(group->child(i), groupName))
            return true;
    }

    return group->text(0) == groupName;
}

QTreeWidgetItem *MainWindow::GetTreetemByName(const QString &name, QTreeWidgetItem *current) const
{
    for(int i = 0; i < current->childCount(); i++)
    {
        auto item = GetTreetemByName(name, current->child(i));

        if(item != 0)
            return item;
    }

    return current->text(0) != name ? 0 : current;
}

void MainWindow::on_groupAdd_PushButton_clicked()
{
    ui->groupsTree_TreeWidget->addTopLevelItem(CreateGroup("Новая группа"));
}

void MainWindow::RemoveGroup(QTreeWidgetItem *item)
{
    while(item->childCount() != 0)
        RemoveGroup(item->child(0));

    renderer->GetScene()->ResetGroup(item);

    delete item;
}

void MainWindow::on_groupRemove_PushButton_clicked()
{
    RemoveGroup(ui->groupsTree_TreeWidget->currentItem());
}

void MainWindow::on_groupSelect_PushButton_clicked()
{
    renderer->GetScene()->ResetSelection();
    renderer->GetScene()->ToogleGroupSelection(ui->groupsTree_TreeWidget->currentItem());

    Point2D selectionPosition = renderer->GetScene()->GetSelectionCenter(PointModeEnabled());
    UpdateSelectionPositionInput(selectionPosition.x, selectionPosition.y, renderer->GetScene()->HasSelection(PointModeEnabled()));

    renderer->repaint();
}

void MainWindow::on_groupMerge_PushButton_clicked()
{
    renderer->GetScene()->SetSelectionGroup(ui->groupsTree_TreeWidget->currentItem());
}

void MainWindow::on_groupUngroup_PushButton_clicked()
{
    //renderer->GetScene()->SetSelectionGroup(0);
    renderer->GetScene()->ResetGroup(ui->groupsTree_TreeWidget->currentItem());
}


//PERFORMS



void MainWindow::on_applyPerform_PushButton_clicked()
{
    if(!renderer->GetScene()->HasSelection(PointModeEnabled()))
        return;

    Point2D center = renderer->GetScene()->GetSelectionCenter(PointModeEnabled());

    QMatrix4x4 firstTranslate, translate, rotate, scale, endTranslate;

    if(ui->performByCenter_RadioButton->isChecked())
        firstTranslate.translate(-center.x, -center.y);

    translate.translate(ui->performTransferX_DoubleSpinBox->value(), ui->performTransferY_DoubleSpinBox->value());

    rotate.rotate(ui->performRotate_DoubleSpinBox->value(), 0, 0, 1);

    scale.scale(ui->performScaleX_DoubleSpinBox->value(), ui->performScaleY_DoubleSpinBox->value());

    if(ui->performByCenter_RadioButton->isChecked())
        endTranslate.translate(center.x, center.y);

    renderer->GetScene()->ApplyMatrix(
                endTranslate * scale * rotate * translate * firstTranslate,
                //firstTranslate * translate * rotate * scale * endTranslate,
                PointModeEnabled());

    Point2D selectionPosition = renderer->GetScene()->GetSelectionCenter(PointModeEnabled());

    UpdateSelectionPositionInput(selectionPosition.x, selectionPosition.y, true);
    renderer->repaint();
}

void MainWindow::on_cleanPerform_PushButton_clicked()
{
    ui->performTransferX_DoubleSpinBox->setValue(0);
    ui->performTransferY_DoubleSpinBox->setValue(0);
    ui->performRotate_DoubleSpinBox->setValue(0);
    ui->performScaleX_DoubleSpinBox->setValue(1);
    ui->performScaleY_DoubleSpinBox->setValue(1);
}

bool MainWindow::LoadSceneState(const QByteArray &data)
{
    QJsonObject root = QJsonDocument::fromBinaryData(data).object();

    if(root.isEmpty())
        return false;

    if(!root.contains("groups") || !root.contains("lines"))
        return false;

    QJsonArray groupsArray = root["groups"].toArray();

    for(int i = 0; i < groupsArray.count(); i++)
    {
        QJsonArray group = groupsArray[i].toArray();

        if(group.size() != 2)
            return false;

        QString groupName = group.at(0).toString();
        QString parentName = group.at(1).toString();

        auto item = CreateGroup(groupName);

        auto tree = ui->groupsTree_TreeWidget;

        if(!parentName.isEmpty())
            for(int i = 0; i < tree->topLevelItemCount(); i++)
            {
                QTreeWidgetItem *parentItem = GetTreetemByName(parentName, tree->topLevelItem(i));

                if(parentItem)
                {
                    parentItem->addChild(item);
                    break;
                }
            }
        else
            ui->groupsTree_TreeWidget->addTopLevelItem(item);
    }

    QJsonArray linesArray = root["lines"].toArray();

    if(!renderer->GetScene()->LoadFromJson(linesArray, ui->groupsTree_TreeWidget))
        return false;

    renderer->GetScene()->UnloadDataToRenderer(renderer);

    return true;
}

void MainWindow::on_importScene_Action_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт сцены"),
                                                    QDir::homePath(),
                                                    tr("Файлы схемы сцен .scene (*.scene);;Все файлы (*)"));

    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        auto data = file.readAll();
        file.close();
        on_addScene_Action_triggered();
        if(!LoadSceneState(data))
        {
            on_removeScene_Action_triggered();
            QMessageBox::critical(this, "Импорт сцены", "Файл поврежден или имеет неизвестный формат!");
        }
    }
    else
    {
        QMessageBox::critical(this, "Импорт сцены", "Невозможно открыть файл!");
    }
}

void MainWindow::GroupToJson(QTreeWidgetItem *item, QJsonArray *groupData) const
{
    QJsonArray group;

    group.append(QJsonValue(item->text(0)));
    group.append(QJsonValue(item->parent() ? item->parent()->text(0) : ""));

    groupData->append(group);

    for(int i = 0; i < item->childCount(); i++)
        GroupToJson(item->child(i), groupData);

}

QByteArray MainWindow::SaveSceneState() const
{
    QJsonArray groupData;

    for(int i = 0; i < ui->groupsTree_TreeWidget->topLevelItemCount(); i++)
    {
        GroupToJson(ui->groupsTree_TreeWidget->topLevelItem(i), &groupData);
    }

    QJsonArray lineData = renderer->GetScene()->SaveToJson();

    //qDebug() << lineData;
    //qDebug() << groupData;

    QJsonObject root;

    root.insert("groups", groupData);
    root.insert("lines", lineData);

    return QJsonDocument(root).toBinaryData();
}

void MainWindow::on_exportScene_Action_triggered()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Экспорт сцены"),
                                                    QDir::homePath(),
                                                    tr("Файлы схемы сцен .scene (*.scene);;Все файлы (*)"));
    if(fileName.isEmpty())
        return;

    if(!fileName.endsWith(".scene"))
        fileName.append(".scene");


    auto data = SaveSceneState();

    QFile file(fileName);
    if(file.open(QFile::WriteOnly))
    {
        file.write(data);
        file.close();
    }
}

void MainWindow::on_morphingSetStartPoint_PushButton_clicked()
{
    renderer->GetScene()->SetMorphingStart();
    ui->morphingSlider_ScrollBar->setValue(0);
}

void MainWindow::on_morphingSetEndPoint_PushButton_clicked()
{
    renderer->GetScene()->SetMorphingEnd();
    ui->morphingSlider_ScrollBar->setValue(100);
}

void MainWindow::on_morphingReset_PushButton_clicked()
{
    renderer->GetScene()->ResetMorphing();
}

void MainWindow::on_morphingSlider_ScrollBar_valueChanged(int value)
{
    renderer->SetMorphingState((float)value / 100);
}

void MainWindow::on_startCreateLine_Action_triggered()
{
    EnableCreateLineMode();
}

void MainWindow::on_startCreateFractal_Action_triggered()
{
    CreateFractalDialog *dialog = new CreateFractalDialog(this);

    dialog->exec();

    if(dialog->result() == QDialog::Accepted)
    {
        QTreeWidgetItem *item = CreateGroup("Фрактальное дерево");

        ui->groupsTree_TreeWidget->addTopLevelItem(item);

        renderer->GetScene()->MergeGroup(dialog->GetLines(), item);
        renderer->GetScene()->UnloadDataToRenderer(renderer);
    }

    dialog->deleteLater();
}

void MainWindow::on_quitApplication_Action_triggered()
{
    QApplication::quit();
}

void MainWindow::on_enterMatrix_PushButton_clicked()
{
    if(enterMatrixDialog && enterMatrixDialog->isVisible())
        return;

    if(!enterMatrixDialog)
        enterMatrixDialog = new EnterTheMatrixDialog(this, renderer);

    enterMatrixDialog->show();
}
