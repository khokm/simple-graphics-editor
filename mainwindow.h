#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "scene.h"
#include "renderer.h"
#include <QMainWindow>
#include <QComboBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QStringListModel>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <createfractaldialog.h>
#include "enterthematrixdialog.h"
//#define KEY_W 25
//#define KEY_E 26
//#define KEY_R 27
//#define CONTROL_KEY 37
//#define SHIFT_KEY 50
//#define ESCAPE_KEY 9

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void UpdateViewPositionInput(float viewX, float viewY);
    void UpdateViewScaleInput(float scale);

    void UpdateSelectionPositionInput(float selectionX, float selectionY, bool enabled);

    void ToogleSelectionMoveButton();
    void ToogleSelectionRotateButton();
    void ToogleSelectionScaleButton();
    void TooglePointModeButton();
    void UntoogleAllSelectionButtons();

    void UpdateBottomPanelPosition();
    void UpdateMorphingPanel();

    SelectionMode GetSelectionMode() const;

    bool PointModeEnabled() const;

    void EnableCreateLineMode();
    void DisableCreateLineMode();
private:
    class EnterTheMatrixDialog *enterMatrixDialog = 0;
    Ui::MainWindow *ui;
    class Renderer *renderer;
    QString groupsTree_prevItemText;
    void RemoveGroup(QTreeWidgetItem *item);

    void GroupToJson(QTreeWidgetItem *item, QJsonArray *groupData) const;
    bool GroupExist(QTreeWidgetItem *group, const QString &groupName) const;
    QTreeWidgetItem *GetTreetemByName(const QString &name, QTreeWidgetItem *first) const;

    QByteArray SaveSceneState() const;
    bool LoadSceneState(const QByteArray &data);
    QTreeWidgetItem *CreateGroup(const QString &name) const;

private slots:
    void on_selectionMove_Action_toggled(bool arg1);
    void on_selectionRotate_Action_toggled(bool arg1);
    void on_selectionScale_Action_toggled(bool arg1);

    void OnSelectionCoords_EditingFinished();
    void OnViewCoords_EditingFinished();
    void OnViewScale_EditingFinished();

    void on_addScene_Action_triggered();
    void OnCurrentScene_IndexChanged(int);
    void on_removeScene_Action_triggered();
    void on_renameScene_Action_triggered();

    void on_setLineWidth_Action_triggered();
    void on_createCustomLine_PushButton_clicked();
    void on_lineMode_Action_triggered();
    void on_pointMode_Action_triggered();

    void on_groupsTree_TreeWidget_itemSelectionChanged();
    void on_groupsTree_TreeWidget_itemChanged(QTreeWidgetItem *item, int);

    void on_groupAdd_PushButton_clicked();
    void on_groupRemove_PushButton_clicked();
    void on_groupSelect_PushButton_clicked();
    void on_groupMerge_PushButton_clicked();
    void on_groupUngroup_PushButton_clicked();

    void on_applyPerform_PushButton_clicked();
    void on_cleanPerform_PushButton_clicked();

    void on_importScene_Action_triggered();
    void on_exportScene_Action_triggered();
    void on_morphingSetStartPoint_PushButton_clicked();
    void on_morphingSetEndPoint_PushButton_clicked();
    void on_morphingReset_PushButton_clicked();
    void on_morphingSlider_ScrollBar_valueChanged(int value);
    void on_startCreateLine_Action_triggered();
    void on_startCreateFractal_Action_triggered();
    void on_quitApplication_Action_triggered();
    void on_enterMatrix_PushButton_clicked();
};

#endif // MAINWINDOW_H
