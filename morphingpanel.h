#ifndef MORPHINGPANEL_H
#define MORPHINGPANEL_H
#include <QDockWidget>
#include <QGridLayout>
#include "mainwindow.h"

class MorphingPanel : public QDockWidget
{
    Q_OBJECT
public:
    explicit MorphingPanel(QWidget *parent = 0);
protected:
    void resizeEvent(QResizeEvent *);
private:
    MainWindow *mainWindow;
};

#endif // MORPHINGPANEL_H
