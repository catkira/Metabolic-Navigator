#include <QHeaderView>
#include "browsecompoundswidget.h"
#include "Compound.h"
#include "Pool.h"
#include "CompoundTransition.h"
#include "defines.h"
#include <QCloseEvent>

using namespace std;

BrowseCompoundsWidget::BrowseCompoundsWidget(QWidget *parent, Pool *pool, MyAlgorithms *myAlgorithms)
    : QDockWidget(parent), pool(pool), myAlgorithms(myAlgorithms)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"ID"<<"Name"<<"Formula"<<"Inchi";
    ui.tableWidget->setHorizontalHeaderLabels(labels);

}

BrowseCompoundsWidget::~BrowseCompoundsWidget()
{
    transitionView.close();
}

void BrowseCompoundsWidget::update(unsigned int signal)
{
    if(signal == UPDATE_COMPOUNDS)
    {
        on_pushButton_2_clicked();
    }
}

void BrowseCompoundsWidget::closeEvent ( QCloseEvent * event )
{
    transitionView.close();
    event->accept();
}

void BrowseCompoundsWidget::showSelectedCompounds(void)
{
    on_pushButton_3_clicked();
}

void BrowseCompoundsWidget::on_pushButton_3_clicked()
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

void BrowseCompoundsWidget::on_pushButton_2_clicked()
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

void BrowseCompoundsWidget::on_pushButton_4_clicked()
{
    transitionView.show();
}

void BrowseCompoundsWidget::updateCompoundDisplay(void)
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
        if(!currentCompound->names.empty())
            twi->setText(currentCompound->names.begin()->c_str());
        ui.tableWidget->setItem(n,1,twi);

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].formula.text().c_str());
        twi->setText(currentCompound->formula.text().c_str());
        ui.tableWidget->setItem(n,2,twi);

        twi=new QTableWidgetItem();
        //twi->setText(QString().setNum((*pool->compounds)[index].isomeres.size()));
        twi->setText(QString(currentCompound->inchi.c_str()));
        ui.tableWidget->setItem(n,3,twi);
    }
    ui.tableWidget->resizeRowsToContents();

}

void BrowseCompoundsWidget::on_tableWidget_cellDoubleClicked(int a,int b)
{
    unsigned int index=ui.tableWidget->currentRow();
    emit compoundSelected(QString(displayCompounds[index].second->cNumber.c_str()));

}

void BrowseCompoundsWidget::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    //string currentCompoundID = displayCompounds[index].second->cNumber;
    Compound *currentCompound = displayCompounds[index].second;
    T_compoundTransitionsIterator compoundTransitionIterator;
    ui.listWidget_3->clear();
    currentCompoundTransitions.clear();
    compoundTransitionIterator = pool->compoundTransitions.begin();
    while(compoundTransitionIterator != pool->compoundTransitions.end())
    {
        if(compoundTransitionIterator->second.compoundPair.first == currentCompound ||
            compoundTransitionIterator->second.compoundPair.second == currentCompound)
        {
            ui.listWidget_3->addItem(QString(
                string(compoundTransitionIterator->second.compoundPair.first->cNumber + " <-> " + compoundTransitionIterator->second.compoundPair.second->cNumber
                + " in reaction " + compoundTransitionIterator->second.inReactionsID.at(0)).c_str()));
            currentCompoundTransitions.push_back(&(compoundTransitionIterator->second));
        }
        else if(compoundTransitionIterator->second.compoundPairID.first == currentCompound->cNumber ||
            compoundTransitionIterator->second.compoundPairID.second == currentCompound->cNumber)
        {
            ui.listWidget_3->addItem(QString(
                string(compoundTransitionIterator->second.compoundPairID.first + " <-> " + compoundTransitionIterator->second.compoundPairID.second
                + " in reaction " + compoundTransitionIterator->second.inReactionsID.at(0)).c_str()));
            currentCompoundTransitions.push_back(&(compoundTransitionIterator->second));
        }
        ++compoundTransitionIterator;
    }

    ui.listWidget->clear();
    vector<string>::iterator namesIterator = displayCompounds[index].second->names.begin();
    while(namesIterator != displayCompounds[index].second->names.end())
    {
        ui.listWidget->addItem(QString(namesIterator->c_str()));
        ++namesIterator;
    }

    QString tempText;
    vector<vector<int>>::iterator vIterator;

    vIterator = displayCompounds[index].second->homotopes.begin();
    while(vIterator != displayCompounds[index].second->homotopes.end())
    {
        tempText += "(";
        for(unsigned int n=0;n<vIterator->size();n++)
        {
            if(n>0)
                tempText += "," + QString().setNum(vIterator->at(n));
            else
                tempText += QString().setNum(vIterator->at(n));
        }
        tempText += "); ";
        ++vIterator;
    }
    ui.lineEdit_2->setText(tempText);

    tempText.clear();
    vIterator = displayCompounds[index].second->prochirales.begin();
    while(vIterator != displayCompounds[index].second->prochirales.end())
    {
        tempText += "(";
        for(unsigned int n=0;n<vIterator->size();n++)
        {
            if(n>0)
                tempText += "," + QString().setNum(vIterator->at(n));
            else
                tempText += QString().setNum(vIterator->at(n));
        }
        tempText += "); ";
        ++vIterator;
    }
    ui.lineEdit->setText(tempText);

}

void BrowseCompoundsWidget::on_listWidget_3_itemSelectionChanged()
{
    transitionView.selectTransition(currentCompoundTransitions.at(ui.listWidget_3->currentRow()));
}