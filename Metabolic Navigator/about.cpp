#include "about.h"

About::About(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

About::~About()
{

}


void About::on_pushButton_clicked()
{
    close();
}