#ifndef SELECTLABELWIDGET_H
#define SELECTLABELWIDGET_H

#include <QDialog>
#include "ui_selectlabelwidget.h"

class SelectLabelWidget : public QDialog
{
    Q_OBJECT

public:
    SelectLabelWidget(QWidget *parent = 0);
    ~SelectLabelWidget();

private:
    Ui::SelectLabelWidgetClass ui;
};

#endif // SELECTLABELWIDGET_H
