#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include "ui_progressdialog.h"

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

public slots:
    void setCurrentCount(unsigned int currentCount);
    void setTitle(const QString& title);
    void setInfoText(const QString& infoText);
    void setEndCount(const unsigned int& endCound);

private:
    Ui::ProgressDialogClass ui;
};

#endif // PROGRESSDIALOG_H
