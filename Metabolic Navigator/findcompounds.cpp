#include "findcompounds.h"

using namespace std;

FindCompounds::FindCompounds(QWidget *parent, Options *options)
    : QDialog(parent)
{
    ui.setupUi(this);
    FindCompounds::options = options;
    if(options!=0)
        options->loadWindowOptions(this, "FindCompounds");
    ui.radioButtonKeggID->setChecked(true);
    connect(ui.lineEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));
    connect(ui.lineEdit_3,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));
}

FindCompounds::~FindCompounds()
{

}


void FindCompounds::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "FindCompounds");
    event->accept();
}


void FindCompounds::on_radioButtonFormula_toggled(bool state)
{
    if(state)
    {
        ui.groupBoxKeggID->setEnabled(false);
        ui.groupBoxFormula->setEnabled(true);
    }
}

void FindCompounds::on_radioButtonName_toggled(bool state)
{
    if(state)
    {
        ui.groupBoxKeggID->setEnabled(true);
        ui.groupBoxFormula->setEnabled(false);
    }
}

void FindCompounds::on_radioButtonKeggID_toggled(bool state)
{
    if(state)
    {
        ui.groupBoxKeggID->setEnabled(true);
        ui.groupBoxFormula->setEnabled(false);
    }
}

void FindCompounds::on_pushButton_clicked()
{
    if(ui.radioButtonKeggID->isChecked())
    {
        set<Compound*> found;
        bool unique=false;
        myAlgorithms->findCompounds(QStringList(ui.lineEdit->text()),found,unique);
    }
    else if(ui.radioButtonFormula->isChecked())
    {
        string pattern = string(ui.lineEdit_3->text().toLatin1());
        myAlgorithms->findCompoundByFormula(pattern);
    }
    emit showSelectedCompounds();

}