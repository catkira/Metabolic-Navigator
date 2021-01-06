#include "scopecheck.h"
#include <QFileDialog>

ScopeCheck::ScopeCheck(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

ScopeCheck::~ScopeCheck()
{

}


void ScopeCheck::on_textBrowser_2_textChanged()
{

}

void ScopeCheck::on_pushButton_2_clicked()
{
    QString logFileName= QFileDialog::getOpenFileName (0,"Select metabolite list","D:\\rtrace_snipped.txt","*.*",0);
    if(logFileName.length() > 0 )
    {
    }

}