#ifndef ENTERTHEMATRIXDIALOG_H
#define ENTERTHEMATRIXDIALOG_H

#include <QDialog>
#include <QMatrix4x4>
#include "renderer.h"

namespace Ui {
class EnterTheMatrixDialog;
}

class EnterTheMatrixDialog : public QDialog
{
    Q_OBJECT

public:
    EnterTheMatrixDialog(QWidget *parent, class Renderer *renderer);
    ~EnterTheMatrixDialog();

private slots:
    void on_EnterDefault_clicked();
    void on_ApplyPerform_clicked();

private:
    class MainWindow *parent;
    class Renderer *renderer;
    Ui::EnterTheMatrixDialog *ui;
};

#endif // ENTERTHEMATRIXDIALOG_H
