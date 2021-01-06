#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include "ui_logwidget.h"

class LogWidget : public QDockWidget
{
    Q_OBJECT

public:
    LogWidget(QWidget *parent = 0);
    ~LogWidget();

private:
    Ui::LogWidgetClass ui;

public slots:
    void addLogLine(QString);
};

#endif // LOGWIDGET_H
