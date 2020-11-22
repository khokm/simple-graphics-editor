#include "enterthematrixdialog.h"
#include "ui_enterthematrixdialog.h"

EnterTheMatrixDialog::EnterTheMatrixDialog(QWidget *parent, Renderer *renderer) :
    QDialog(parent),
    ui(new Ui::EnterTheMatrixDialog)
{
    ui->setupUi(this);
    this->renderer = renderer;
    this->parent = (MainWindow*)parent;
}

EnterTheMatrixDialog::~EnterTheMatrixDialog()
{
    delete ui;
}

void EnterTheMatrixDialog::on_EnterDefault_clicked()
{
    ui->m11->setValue(1);
    ui->m12->setValue(0);
    ui->m13->setValue(0);

    ui->m21->setValue(0);
    ui->m22->setValue(1);
    ui->m23->setValue(0);

    ui->m31->setValue(0);
    ui->m32->setValue(0);
    ui->m33->setValue(1);
}

void EnterTheMatrixDialog::on_ApplyPerform_clicked()
{
    QMatrix4x4 matrix(
                ui->m11->value(), ui->m12->value(), 0, ui->m13->value(),
                ui->m21->value(), ui->m22->value(), 0, ui->m23->value(),
                0,                  0,              1,                0,
                ui->m31->value(), ui->m32->value(), 0,ui->m33->value()
                );

    renderer->GetScene()->ApplyMatrix(matrix.transposed(), parent->PointModeEnabled());
    renderer->repaint();
}
