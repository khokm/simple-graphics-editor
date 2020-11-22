#include "createfractaldialog.h"
#include "ui_createfractaldialog.h"

CreateFractalDialog::CreateFractalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateFractalDialog)
{
    ui->setupUi(this);
    on_pushButton_2_clicked();
}

CreateFractalDialog::~CreateFractalDialog()
{
    delete ui;
}

std::vector<Line2D> *CreateFractalDialog::GetLines()
{
    return &ui->openGLWidget->lines;
}

void CreateFractalDialog::RebuildTree()
{
    if(antilag)
        return;

    int deep = ui->treeDeep->value();
    int branchCount = ui->branchCount->value();

    int total = 0;
    ui->openGLWidget->BuildTree(deep, branchCount, ui->firstRight->isChecked(), ui->branchAngle->value(), total);
    ui->totalCountLabel->setText(QString("Суммарное количество ветвей: ").append(QString::number(total)));
}

void CreateFractalDialog::on_pushButton_2_clicked()
{
    antilag = true;
    ui->treeDeep->setValue(3 + rand() % 5);
    ui->branchCount->setValue(2 + rand() % 6);
    ui->firstRight->setChecked(rand() % 2);
    ui->branchAngle->setValue(RandomFloat(10, 30));
    antilag = false;
    RebuildTree();
}

void CreateFractalDialog::on_createAndExit_clicked()
{
    accept();



    close();
}
