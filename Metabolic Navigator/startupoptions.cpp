#include "startupoptions.h"
#include "Options.h"

StartupOptions::StartupOptions(QWidget *parent, Options *options)
    : QDialog(parent), options(options)
{
    ui.setupUi(this);
    if(options!=NULL)
    {
        ui.checkBox->setChecked(options->settings->value("savePos").toInt());
        ui.checkBox_4->setChecked(options->settings->value("loadKegg").toInt());
        ui.checkBox_5->setChecked(options->settings->value("loadFatemaps").toInt());
    }

}

StartupOptions::~StartupOptions()
{
    if(options != 0)
    {
        options->settings->setValue("savePos", (int)ui.checkBox->isChecked());
        options->settings->setValue("loadKegg", (int)ui.checkBox_4->isChecked());
        options->settings->setValue("loadFatemaps", (int)ui.checkBox_5->isChecked());
    }
}


void StartupOptions::on_pushButton_clicked()
{
    close();
}

void StartupOptions::on_pushButton_2_clicked()
{
    close();
}