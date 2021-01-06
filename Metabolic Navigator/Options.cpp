#include "Options.h"

Options::Options(void)
{
}

Options::~Options(void)
{
}

void Options::saveWindowOptions(QWidget *widget, QString name)
{
    settings->setValue(name+"/geometry", widget->saveGeometry());
    settings->setValue(name+"/isVisible", widget->isVisible());
}

void Options::loadWindowOptions(QWidget *widget, QString name)
{
    widget->restoreGeometry(settings->value(name+"/geometry").toByteArray()); 
    if(settings->value(name+"/isVisible")==true)
        widget->show();
}
