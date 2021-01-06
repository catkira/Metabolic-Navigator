#include "browsedialog.h"
#include <QHeaderView>
#include "userdataset.h"
#include "CompoundTransition.h"
#include "Options.h"
#include "Pool.h"
#include "MyAlgorithms.h"

using namespace std;

BrowseDialog::BrowseDialog(QWidget *parent, Options *options, Pool *pool, MyAlgorithms *myAlgorithms)
    : QDialog(parent), pool(pool), myAlgorithms(myAlgorithms), options(options)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"C-Number"<<"Name"<<"Formula"<<"#isomeres";
    ui.tableWidget->setHorizontalHeaderLabels(labels);
    labels.clear();
    labels<<"R-Number"<<"Equation"<<"Source"<<"Compartments";
    ui.tableWidget_2->setHorizontalHeaderLabels(labels);
    if(options!=0)
        options->loadWindowOptions(this, "BrowseDialog");

    connect(ui.tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sectionCompoundClicked(int)));
    connect(ui.tableWidget_2->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sectionReactionClicked(int)));

}

BrowseDialog::~BrowseDialog()
{

}

void BrowseDialog::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "BrowseDialog");
    transitionView.close();
    event->accept();
}

void BrowseDialog::showSelectedReactions(void)
{
    on_pushButton_6_clicked();
}

void BrowseDialog::showSelectedCompounds(void)
{
    on_pushButton_3_clicked();
}

void BrowseDialog::updateCompoundDisplay(void)
{
    QTableWidgetItem *twi;
    ui.tableWidget->setRowCount(displayCompounds.size());
    unsigned int index;
    Compound *currentCompound;
    for(unsigned int n=0;n<displayCompounds.size();n++)
    {
        //index = displayCompounds[n].second;
        currentCompound = displayCompounds[n].second;

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].cNumber.c_str());
        twi->setText(currentCompound->cNumber.c_str());
        ui.tableWidget->setItem(n,0,twi);

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].names[0].c_str());
        //twi->setText(currentCompound->names[0].c_str());
        twi->setText(currentCompound->names.begin()->c_str());
        ui.tableWidget->setItem(n,1,twi);

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].formula.text().c_str());
        twi->setText(currentCompound->formula.text().c_str());
        ui.tableWidget->setItem(n,2,twi);

        twi=new QTableWidgetItem();
        //twi->setText(QString().setNum((*pool->compounds)[index].isomeres.size()));
        twi->setText(QString().setNum(currentCompound->isomeres.size()));
        ui.tableWidget->setItem(n,3,twi);
    }
    ui.tableWidget->resizeRowsToContents();
}

void BrowseDialog::updateReactionDisplay(void)
{
    QTableWidgetItem *twi;
    QString temp;
    set<string>::iterator globalCompartmentsIterator;
    unsigned int i;
    unsigned int index;
    unsigned int displayReactionsSize=displayReactions.size();

    ui.tableWidget_2->setRowCount(displayReactions.size());
    Reaction *reaction;
    for(unsigned int n=0;n<displayReactionsSize;n++)
    {
        reaction = displayReactions[n].second;

        twi=new QTableWidgetItem(reaction->id.c_str(),0);
        ui.tableWidget_2->setItem(n,0,twi);

        twi=new QTableWidgetItem(reaction->equation->text().c_str(),0);
        ui.tableWidget_2->setItem(n,1,twi);

        twi=new QTableWidgetItem(reaction->source.c_str(),0);
        if(reaction->dataset != -1)
        {
            if((*pool->userDatasets)[reaction->dataset].accepted==true)        
                twi->setBackground(Qt::green);
            else
                twi->setBackground(Qt::yellow);
        }
        ui.tableWidget_2->setItem(n,2,twi);

        if(reaction->dataset==-1)
        {
            twi=new QTableWidgetItem();
            twi->setText("");
            ui.tableWidget_2->setItem(n,3,twi);
            continue;
        }
        twi=new QTableWidgetItem();
        temp="";
        //if((*pool->reactions)[index].equation->compartments.size()!=0)
        if(reaction->equation->hasMetaboliteCompartments() == true)
        {
            temp="-> see compounds";
        }
        else
        {
            globalCompartmentsIterator = reaction->equation->globalCompartments.begin();
            for(unsigned int m=0;m<reaction->equation->globalCompartments.size();m++)
            {
                temp+=QString(globalCompartmentsIterator->c_str()) + " ";
                globalCompartmentsIterator++;
            }
        }
        twi->setText(temp);
        ui.tableWidget_2->setItem(n,3,twi);
    }
    ui.tableWidget_2->resizeRowsToContents();
}

void BrowseDialog::on_pushButton_2_clicked()
{
    displayCompounds.clear();
    map<string,Compound>::iterator compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        displayCompounds.push_back(pair<string,Compound*> (string(""),&(compoundsIterator->second)));
        compoundsIterator++;
    }
    updateCompoundDisplay();
}

void BrowseDialog::on_pushButton_3_clicked()
{
    displayCompounds.clear();
    set<string>::iterator selectedCompoundsIterator = pool->selectedCompounds->begin();
    while(selectedCompoundsIterator != pool->selectedCompounds->end())
    {
        displayCompounds.push_back(pair<string,Compound*> (string(""),&(*pool->compounds)[*selectedCompoundsIterator]));
        selectedCompoundsIterator++;
    }
    updateCompoundDisplay();
}

void BrowseDialog::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    ui.listWidget->clear();
    ui.listWidget_2->clear();
    ui.listWidget_3->clear();
    map<Reaction *, unsigned int>::iterator linkedReactionsIterator;
    T_compoundTransitionsIterator compoundTransitionsIterator;

    myAlgorithms->validateLinkedReactions();
    if(displayCompounds.size()>=index)
    {
        Compound *currentCompound = displayCompounds[index].second;
        set<string>::iterator namesIterator;
        namesIterator = displayCompounds[index].second->names.begin();
        while(namesIterator != displayCompounds[index].second->names.end())
        {
            ui.listWidget->addItem(QString(namesIterator->c_str()));
            ++namesIterator;
        }

        /*
        for(unsigned int n=0;n<displayCompounds[index].second->names.size();n++)
        {
            ui.listWidget->addItem(QString(displayCompounds[index].second->names[n].c_str()));
        }
        */
        linkedReactionsIterator = displayCompounds[index].second->linkedReactions.begin();
        for(unsigned int n=0;n<displayCompounds[index].second->linkedReactions.size();n++)
        {
            ui.listWidget_2->addItem(QString(linkedReactionsIterator->first->id.c_str()));
            linkedReactionsIterator++;
        }
        compoundTransitionsIterator = pool->compoundTransitions.begin();
        string temp;
        displayedCompoundTransitions.clear();
        while(compoundTransitionsIterator != pool->compoundTransitions.end())
        {
            if(compoundTransitionsIterator->compoundPairID.first == currentCompound->cNumber ||
                compoundTransitionsIterator->compoundPairID.second == currentCompound->cNumber)
            {
                compoundTransitionsIterator->generateIDText(temp);
                ui.listWidget_3->addItem(QString(temp.c_str()));
                displayedCompoundTransitions.push_back(&(*compoundTransitionsIterator));
            }
            ++compoundTransitionsIterator;
        }

    }
}

void BrowseDialog::on_tableWidget_cellDoubleClicked(int,int)
{
    unsigned int index=ui.tableWidget->currentRow();
    emit compoundSelected(QString(displayCompounds[index].second->cNumber.c_str()));
}

void BrowseDialog::on_pushButton_clicked()
{
    displayReactions.clear();
    for(unsigned int n=0;n<pool->reactions->size();n++)
        displayReactions.push_back(pair<string,Reaction*> (string(""),&(*pool->reactions)[n]));
    updateReactionDisplay();
}

void BrowseDialog::on_pushButton_6_clicked()
{
    displayReactions.clear();

    set<Reaction*>::iterator selectedReactionsIterator;
    selectedReactionsIterator = pool->selectedReactions2->begin();
    while(selectedReactionsIterator != pool->selectedReactions2->end())
    {
        displayReactions.push_back(pair<string,Reaction*> (string(""),
            *selectedReactionsIterator));
        ++selectedReactionsIterator;
    }

    updateReactionDisplay();
}

void BrowseDialog::on_tableWidget_2_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget_2->currentRow();
    QString tempString;
    map<pair<string,string>,string>::iterator rpairIterator;
    if(displayReactions.size()>=index)
    {
        ui.lineEdit->setText(displayReactions[index].second->equation->text(ui.checkBox_2->isChecked(), true, pool).c_str());
        ui.lineEdit_2->setText(displayReactions[index].second->names.begin()->c_str());
        if(ui.checkBox->isChecked())
            emit showDataset(displayReactions[index].second->dataset);
        rpairIterator = displayReactions[index].second->rpair.begin();
        ui.comboBox->clear();
        for(unsigned int n=0;n<displayReactions[index].second->rpair.size();n++)
        {
            tempString = string(rpairIterator->first.first + string(" ") +
                rpairIterator->first.second + string(" ") + rpairIterator->second ).c_str();
            ui.comboBox->insertItem(n,tempString);
            rpairIterator++;
        }
    }
    else
    {
        ui.lineEdit->clear();
        ui.lineEdit_2->clear();
        ui.comboBox->clear();
    }

}

void BrowseDialog::sectionCompoundClicked ( int logicalIndex )
{
}

void BrowseDialog::sectionReactionClicked ( int logicalIndex )
{
    if(logicalIndex==3)
        return;
    vector<pair<string,Reaction*>> oldDisplayReactions = displayReactions;
    displayReactions.clear();
    for(unsigned int n=0;n<ui.tableWidget_2->rowCount();n++)
    {
        if(logicalIndex==0)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->id,oldDisplayReactions[n].second));
        if(logicalIndex==1)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->equation->text(),oldDisplayReactions[n].second));
        if(logicalIndex==2)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->source,oldDisplayReactions[n].second));
    }
    sortByFirst functor;
    std::sort(displayReactions.begin(), displayReactions.end(),functor);
    updateReactionDisplay();
}


void BrowseDialog::on_pushButton_4_clicked()
{
    transitionView.show();
    int index = ui.listWidget_3->currentRow();
    if(index != -1)
        transitionView.selectTransition(displayedCompoundTransitions.at(index));
}

void BrowseDialog::on_listWidget_3_currentRowChanged(int index)
{
    if(index != -1)
        transitionView.selectTransition(displayedCompoundTransitions.at(index));
}