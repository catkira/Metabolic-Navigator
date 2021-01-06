#ifndef STARTUPOPTIONS_H
#define STARTUPOPTIONS_H

#include <QDialog>
#include "ui_startupoptions.h"

class Options;

class StartupOptions : public QDialog
{
    Q_OBJECT

public:
    StartupOptions(QWidget *parent = 0, Options *options = 0);
    ~StartupOptions();

private:
    Ui::StartupOptionsClass ui;
    Options *options;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // STARTUPOPTIONS_H
