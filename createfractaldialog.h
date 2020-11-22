#ifndef CREATEFRACTALDIALOG_H
#define CREATEFRACTALDIALOG_H

#include <QDialog>
#include "line2d.h"

namespace Ui {
class CreateFractalDialog;
}

class CreateFractalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateFractalDialog(QWidget *parent = 0);
    ~CreateFractalDialog();

    std::vector<Line2D> *GetLines();

private slots:
    void RebuildTree();

    void on_pushButton_2_clicked();

    void on_createAndExit_clicked();

private:
    bool antilag = false;
    Ui::CreateFractalDialog *ui;
};

#endif // CREATEFRACTALDIALOG_H
