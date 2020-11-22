#include "morphingpanel.h"

MorphingPanel::MorphingPanel(QWidget *parent) : QDockWidget(parent)
{
    mainWindow = (MainWindow*)(parentWidget());
}

void MorphingPanel::resizeEvent(QResizeEvent *)
{
   mainWindow->UpdateMorphingPanel();
}

