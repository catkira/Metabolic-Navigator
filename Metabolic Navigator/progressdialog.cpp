#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle("");
    setInfoText("");
}

ProgressDialog::~ProgressDialog()
{

}


void ProgressDialog::setTitle(const QString& title)
{
    setWindowTitle(title);
}

void ProgressDialog::setInfoText(const QString& infoText)
{
    ui.label->setText(infoText);
}

void ProgressDialog::setEndCount(const unsigned int& endCount)
{
    ui.progressBar->setMaximum(endCount);
}

void ProgressDialog::setCurrentCount(unsigned int currentCount)
{
    ui.progressBar->setValue(currentCount);
}