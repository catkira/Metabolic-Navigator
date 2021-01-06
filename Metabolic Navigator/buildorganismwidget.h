#ifndef BUILDORGANISMWIDGET_H
#define BUILDORGANISMWIDGET_H

#include <QWidget>
#include "ui_buildorganismwidget.h"

class BuildOrganismWidget : public QWidget
{
    Q_OBJECT

public:
    BuildOrganismWidget(QWidget *parent = 0);
    ~BuildOrganismWidget();

private:
    Ui::BuildOrganismWidgetClass ui;
};

#endif // BUILDORGANISMWIDGET_H
