#include "logwidget.h"

LogWidget::LogWidget(QWidget *parent)
    : QDockWidget(parent)
{
    ui.setupUi(this);
}

LogWidget::~LogWidget()
{

}


void LogWidget::addLogLine(QString line)
{
    ui.textEdit->append(line);
}