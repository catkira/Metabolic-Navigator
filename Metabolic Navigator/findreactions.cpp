#include "findreactions.h"
#include "Helper.h"
#include "equation.h"

using namespace std;

FindReactions::FindReactions(QWidget *parent, Options *options)
    : QDialog(parent)
{
    ui.setupUi(this);
    QStringList list;
    list<<"<=>"<<"=>"<<"<="<<"*";
    ui.comboBox->addItems(list);
    FindReactions::options = options;
    if(options!=0)
        options->loadWindowOptions(this, "FindReactions");
    connect(ui.textEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));
    connect(ui.textEdit_2,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));

}

FindReactions::~FindReactions()
{

}

void FindReactions::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "FindReactions");
    event->accept();
}


void FindReactions::on_pushButton_clicked()
{
    /*
    map<unsigned int, Compound*> leftCompounds;
    map<unsigned int, Compound*> rightCompounds;

    leftCompounds.clear();
    rightCompounds.clear();
  
    unsigned int val;
    unsigned int pos;
    */
    unsigned int searchDirection = ui.comboBox->currentIndex();

    vector<MetaboliteObject*> metabolites;
    vector<T_equationMetabolite> equationMetabolites;

    QString temp;

    temp = ui.textEdit->toPlainText();

    //myAlgorithms->findCompound(getCompoundFromMetabolite(temp),&leftCompounds,&ok);
    myAlgorithms->findMetabolites(temp.split('\n'),metabolites);
    if(temp.length() != 0 && metabolites.empty())
    {
        emit addLogLine("metabolite not found in database");
        return;
    }
    MetabolitesToEquationMetabolites(metabolites, equationMetabolites, METABOLITE_IS_LEFT);

    temp = ui.textEdit_2->toPlainText();


    myAlgorithms->findMetabolites(temp.split('\n'),metabolites);
    MetabolitesToEquationMetabolites(metabolites, equationMetabolites, METABOLITE_IS_RIGHT, true);

    if(temp.length() != 0 && metabolites.empty())
    {
        emit addLogLine("metabolite not found in database");
        return;
    }

    unsigned int numFound;
   // pool->selectedReactions2->clear();

    vector<Reaction*> reactionSet;
    bool includeUserReactions = ui.checkBox_datasets->isChecked();
    bool includeNotAccepted = !ui.checkBox_3->isChecked();
    bool includeTransporter = !ui.checkBox_4->isChecked();
    bool includeKeggReactions = ui.checkBox_kegg->isChecked();

    myAlgorithms->buildReactionSet(&reactionSet, 
        includeUserReactions, includeNotAccepted, includeTransporter, includeKeggReactions);

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

/*
int FindReactions::findReactionsAlgo(vector<T_equationMetabolite> metabolites, 
                                     unsigned int eqnDirection, vector<Reaction*> *reactionSet,
                                     Pool *pool)
{
    unsigned int numFound=0;

    vector<T_equationMetabolite>::iterator equationMetabolitesIterator;
    T_reactionsIterator reactionsIterator;
    bool found;

    unsigned int n=0; // legacy stuff
    reactionsIterator = pool->reactions->begin();
    while(reactionsIterator != pool->reactions->end())
    {
        found = true;
        equationMetabolitesIterator = metabolites.begin();
        while(equationMetabolitesIterator != metabolites.end())
        {
            if(!reactionsIterator->second.equation->containsMetabolite(*equationMetabolitesIterator))
            {
                found = false;
                break;
            }
            ++equationMetabolitesIterator;
        }
        if(found)
        {
            (*pool->selectedReactions)[pool->selectedReactions->size()]=n;
            numFound++;
        }
        ++n;
        ++reactionsIterator;
    }
    return numFound;
}
*/

void FindReactions::on_checkBox_stateChanged(int state)
{
    if(state==Qt::Checked)
    {
        ui.checkBox_3->setEnabled(true);
        ui.checkBox_4->setEnabled(true);
        ui.checkBox_5->setEnabled(true);
    }
    else
    {
        ui.checkBox_3->setEnabled(false);
        ui.checkBox_4->setEnabled(false);
        ui.checkBox_5->setEnabled(false);
    }
}