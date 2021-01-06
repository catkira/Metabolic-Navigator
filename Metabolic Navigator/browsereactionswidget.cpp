#include <set>
#include "browsereactionswidget.h"
#include "Reaction.h"
#include "Pool.h"
#include "Equation.h"
#include "UserDataset.h"
#include "Helper.h"
#include "CompoundTransition.h"
#include "defines.h"
#include <algorithm>

using namespace std;

BrowseReactionsWidget::BrowseReactionsWidget(QWidget *parent, Pool *pool, MyAlgorithms *myAlgorithms)
    : QDockWidget(parent), pool(pool), myAlgorithms(myAlgorithms)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"ID"<<"Equation"<<"Source"<<"Compartments"<<"User settings";
    ui.tableWidget->setHorizontalHeaderLabels(labels);

    ui.checkBox_2->setVisible(false);
    ui.checkBox->setVisible(false);
}

BrowseReactionsWidget::~BrowseReactionsWidget()
{

}

void BrowseReactionsWidget::update(unsigned int signal)
{
    if(signal == UPDATE_REACTIONS)
    {
        on_pushButton_clicked();
    }
}



void BrowseReactionsWidget::on_pushButton_6_clicked()
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

void BrowseReactionsWidget::on_pushButton_clicked()
{
    displayReactions.clear();
    T_reactionsIterator reactionsIterator = pool->reactions->begin();
    //for(unsigned int n=0;n<pool->reactions->size();n++)
    while(reactionsIterator != pool->reactions->end())
    {
        displayReactions.push_back(pair<string,Reaction*> (string(""),&(reactionsIterator->second)));
        ++reactionsIterator;
    }
    updateReactionDisplay();
}

void BrowseReactionsWidget::updateReactionDisplay(void)
{
    QTableWidgetItem *twi;
    QString temp;
    set<string>::iterator globalCompartmentsIterator;
    unsigned int i;
    unsigned int index;
    unsigned int displayReactionsSize=displayReactions.size();

    ui.tableWidget->setRowCount(displayReactions.size());
    Reaction *reaction;
    for(unsigned int n=0;n<displayReactionsSize;n++)
    {
        reaction = displayReactions[n].second;

        twi=new QTableWidgetItem(reaction->id.c_str(),0);
        ui.tableWidget->setItem(n,0,twi);

        twi=new QTableWidgetItem(reaction->equation->text().c_str(),0);
        ui.tableWidget->setItem(n,1,twi);

        twi=new QTableWidgetItem(reaction->source.c_str(),0);
        if(reaction->dataset != -1)
        {
            if((*pool->userDatasets)[reaction->dataset].accepted==true)        
                twi->setBackground(Qt::green);
            else
                twi->setBackground(Qt::yellow);
        }
        ui.tableWidget->setItem(n,2,twi);

        if(reaction->dataset==-1)
        {
            twi=new QTableWidgetItem();
            twi->setText("");
            ui.tableWidget->setItem(n,3,twi);
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
        ui.tableWidget->setItem(n,3,twi);
    }
    ui.tableWidget->resizeRowsToContents();



}

void BrowseReactionsWidget::showSelectedReactions(void)
{
    on_pushButton_6_clicked();
}



void BrowseReactionsWidget::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    QString tempString;
    T_rpairsIterator rpairIterator;
    if(displayReactions.size()>=index)
    {
        ui.lineEdit->setText(displayReactions[index].second->equation->text(ui.checkBox_2->isChecked(), true, pool).c_str());
        //if(!displayReactions[index].second->names.empty())
            //ui.lineEdit_2->setText(displayReactions[index].second->names.begin()->c_str());
        if(ui.checkBox->isChecked())
            emit showDataset(displayReactions[index].second->dataset);

        Reaction *currentReaction = displayReactions[index].second;
        T_compoundTransitionsIterator compoundTransitionIterator;
        ui.listWidget_4->clear();
        currentCompoundTransitions.clear();
        compoundTransitionIterator = pool->compoundTransitions.begin();
        vector<string>::iterator tempIt;
        while(compoundTransitionIterator != pool->compoundTransitions.end())
        {
            tempIt = std::find(compoundTransitionIterator->second.inReactionsID.begin(), 
                compoundTransitionIterator->second.inReactionsID.end(),
                currentReaction->id);
            if(tempIt != compoundTransitionIterator->second.inReactionsID.end())
            {
                if(compoundTransitionIterator->second.compoundPair.first != 0 && compoundTransitionIterator->second.compoundPair.first != 0)
                {
                    ui.listWidget_4->addItem(QString(
                        string(compoundTransitionIterator->second.compoundPair.first->cNumber + " <-> " + compoundTransitionIterator->second.compoundPair.second->cNumber
                        + " in reaction " + compoundTransitionIterator->second.inReactionsID.at(0)).c_str()));
                    currentCompoundTransitions.push_back(&(compoundTransitionIterator->second));
                }
            }
            ++compoundTransitionIterator;
        }

        ui.listWidget->clear();
        vector<string>::iterator namesIterator = displayReactions[index].second->names.begin();
        while(namesIterator != displayReactions[index].second->names.end())
        {
            ui.listWidget->addItem(QString(namesIterator->c_str()));
            ++namesIterator;
        }

        /*
        ui.comboBox->clear();
        rpairIterator = pool->rpairs.find(displayReactions[index].second->id);
        if(rpairIterator != pool->rpairs.end())
        {
            unsigned int n=0;
            while(rpairIterator->first == displayReactions[index].second->id)
            {
                ui.comboBox->insertItem(n,rpairIterator->second.text().c_str());
                ++n;
                ++rpairIterator;
            }
        }
        */
    }
    /*
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
    }*/
    else
    {
        ui.lineEdit->clear();
        //ui.lineEdit_2->clear();
        ui.listWidget->clear();
        //ui.comboBox->clear();
    }

}

void BrowseReactionsWidget::sectionReactionClicked ( int logicalIndex )
{
    if(logicalIndex==3)
        return;
    vector<pair<string,Reaction*>> oldDisplayReactions = displayReactions;
    displayReactions.clear();
    for(unsigned int n=0;n<ui.tableWidget->rowCount();n++)
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
