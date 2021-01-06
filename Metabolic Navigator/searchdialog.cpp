#include "searchdialog.h"
#include "Compound.h"
#include <map>
#include <string>
#include "equation.h"
#include "Options.h"
#include "MyAlgorithms.h"
#include "Pool.h"
#include "Organism.h"

using namespace std;

SearchDialog::SearchDialog(QWidget *parent, Pool *pool, Options *options)
    : QDialog(parent), pool(pool), options(options)
{
    ui.setupUi(this);
    QStringList list;
    list<<"<=>"<<"=>"<<"<="<<"*";
    ui.comboBoxDirection->addItems(list);

    if(options!=0)
        options->loadWindowOptions(this, "SearchDialog");

    ui.radioButtonNameID->setChecked(true);
    on_radioButtonNameID_toggled(true);
}

SearchDialog::~SearchDialog()
{

}

void SearchDialog::update(unsigned int signal)
{
    if(signal == UPDATE_ORGANISMS)
    {
        T_organismsIterator organismsIterator;
        ui.comboBox_4->clear();
        organismsIterator = pool->organisms.begin();
        while(organismsIterator != pool->organisms.end())
        {
            ui.comboBox_4->addItem(organismsIterator->second.name().c_str());
            ++organismsIterator;
        }
    }
}


void SearchDialog::on_pushButton_2_clicked()
{
    if(ui.radioButtonNameID->isChecked())
    {
        std::set<Compound*> found;
        bool unique=false;
        myAlgorithms->findCompounds(QStringList(ui.lineEditNameID->text()),found,unique);
    }
    else if(ui.radioButtonFormula->isChecked())
    {
        string pattern = string(ui.lineEditFormula->text().toLatin1());
        myAlgorithms->findCompoundByFormula(pattern);
    }
    emit showSelectedCompounds();
}

void SearchDialog::on_pushButton_clicked()
{
    unsigned int searchDirection = ui.comboBoxDirection->currentIndex();

    vector<MetaboliteObject*> metabolites;
    vector<T_equationMetabolite> equationMetabolites;

    QString temp;

    temp = ui.textEditLeftMetabolites->toPlainText();

    //myAlgorithms->findCompound(getCompoundFromMetabolite(temp),&leftCompounds,&ok);
    myAlgorithms->findMetabolites(temp.split('\n'),metabolites);
    if(temp.length() != 0 && metabolites.empty())
    {
        emit addLogLine("metabolite not found in database");
        return;
    }
    MetabolitesToEquationMetabolites(metabolites, equationMetabolites, METABOLITE_IS_LEFT);

    temp = ui.textEditRightMetabolites->toPlainText();


    myAlgorithms->findMetabolites(temp.split('\n'),metabolites);
    MetabolitesToEquationMetabolites(metabolites, equationMetabolites, METABOLITE_IS_RIGHT, true);

    if(temp.length() != 0 && metabolites.empty())
    {
        emit addLogLine("metabolite not found in database");
        return;
    }

    unsigned int numFound;
   // pool->selectedReactions2->clear();

    set<Reaction*> reactionSet;
    /*
    bool includeUserReactions = ui.checkBox_datasets->isChecked();
    bool includeNotAccepted = !ui.checkBox_onlyAccepted->isChecked();
    bool includeTransporter = !ui.checkBox_noTransporter->isChecked();
    bool includeKeggReactions = ui.checkBox_kegg->isChecked();

    myAlgorithms->buildReactionSet(&reactionSet, 
        includeUserReactions, includeNotAccepted, includeTransporter, includeKeggReactions);
        */
    
    T_organismsIterator organismsIterator = pool->organisms.find(string(ui.comboBox_4->itemText(ui.comboBox_4->currentIndex()).toAscii()));
    if(organismsIterator == pool->organisms.end())
        return;

    reactionSet = organismsIterator->second.reactions;


    numFound = myAlgorithms->findReactions(equationMetabolites, searchDirection, reactionSet);
    if((searchDirection == direction_both || searchDirection == direction_dontCare))
    {
        twistLeftRightInEquationMetabolites(equationMetabolites);
        numFound += myAlgorithms->findReactions(equationMetabolites, searchDirection, reactionSet, true);
    }

    if(numFound==1)
        emit addLogLine(QString().setNum(numFound) + " match");
    else
        emit addLogLine(QString().setNum(numFound) + " matches");
    emit showSelectedReactions();

}

void SearchDialog::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "SearchDialog");
    event->accept();
}


void SearchDialog::on_lineEditNameID_returnPressed()
{
    on_pushButton_2_clicked();
}

void SearchDialog::on_lineEditFormula_returnPressed()
{
    on_pushButton_2_clicked();
}

void SearchDialog::on_lineEditFormula_textChanged(const QString &)
{

}

void SearchDialog::on_radioButtonNameID_toggled(bool isNameID)
{
    if(isNameID)
    {
        ui.lineEditNameID->setEnabled(true);
        ui.lineEditFormula->setEnabled(false);
    }
    else
    {
        ui.lineEditNameID->setEnabled(false);
        ui.lineEditFormula->setEnabled(true);
    }
}