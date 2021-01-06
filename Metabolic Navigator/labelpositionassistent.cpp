#include "labelpositionassistent.h"
#include "Pool.h"
#include "Organism.h"
#include "Reaction.h"
#include "defines.h"
#include "Equation.h"
#include "TreeNode.h"
#include "MetabolicTree.h"
#include <QMessageBox>
#include "math.h"
#include "Options.h"
#include "svgviewer/svgviewer.h"
#include "MetabolitesViewer.h"



#include "boost/numeric/ublas/vector.hpp"


using namespace std;


LabelPositionAssistent::LabelPositionAssistent(QWidget *parent, Pool *pool, MyAlgorithms *myAlgorithms, Options *options)
    : QDockWidget(parent), pool(pool), myAlgorithms(myAlgorithms), options(options)
{
    ui.setupUi(this);

    ui.tab_2->setEnabled(false);
    ui.tab_3->setEnabled(false);
    ui.tab_4->setEnabled(false);

    ui.tabWidget->setCurrentIndex(0);

    connect(&timer, SIGNAL(timeout()), this, SLOT(coordinateJobsAndUpdateStatus()));

    if(options!=NULL)
    {
        QString temp = options->settings->value("userSettings lpa").toString();
        ui.textEdit_2->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        ui.textEdit_2->insertPlainText(temp);
        
        temp = options->settings->value("blacklist lpa").toString();
        ui.textEdit->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        ui.textEdit->insertPlainText(temp);
    }

    metabolitesViewer = new MetabolitesViewer(this);

}

LabelPositionAssistent::~LabelPositionAssistent()
{
    QString temp = ui.textEdit->toPlainText();
    options->settings->setValue("blacklist lpa", temp);
    temp = ui.textEdit_2->toPlainText();
    options->settings->setValue("userSettings lpa", temp);

    delete metabolitesViewer;
}

void LabelPositionAssistent::coordinateJobsAndUpdateStatus(void)
{
    T_MetabolicTreesIterator metabolicTreesIterator = metabolicTrees.begin();
    while(metabolicTreesIterator != metabolicTrees.end())
    {
        if(metabolicTreesIterator->endReason == BTE_NONE)
        {
            metabolicTreesIterator->buildReactionTreeNewThread(0,0,ui.lineEdit->text().toInt(),false);
            updateNetworksTable();
            //metabolicTreesIterator->buildReactionTree(0,0,100,false);
            return;
        }
        else if(metabolicTreesIterator->endReason == BTE_RUNNING)
            return;
        ++metabolicTreesIterator;
    } 

    updateNetworksTable();
    timer.stop();
}

void LabelPositionAssistent::update(unsigned int signal)
{
    if(signal == UPDATE_ORGANISMS)
    {
        T_organismsIterator organismsIterator;
        ui.comboBox->clear();
        organismsIterator = pool->organisms.begin();
        while(organismsIterator != pool->organisms.end())
        {
            ui.comboBox->addItem(organismsIterator->second.name().c_str());
            ++organismsIterator;
        }
    }
}

void LabelPositionAssistent::on_pushButton_2_clicked()
{
    if(ui.comboBox->currentIndex() == -1)
    {
        QMessageBox::critical(this, "Problem","You need to import organism definitions before you can proceed here");
        return;
    }
    
    if(ui.listWidget->count() == 0)
    {
        QMessageBox::critical(this, "Problem","The selected organism has no metabolites");
        return;
    }

    // setup next tab
    ui.tab_2->setEnabled(true);

    ui.tabWidget->setCurrentIndex(1);
}

void LabelPositionAssistent::on_comboBox_currentIndexChanged(int index)
{
    ui.listWidget->clear();

    T_organismsIterator organismsIterator = pool->organisms.find(string(ui.comboBox->itemText(ui.comboBox->currentIndex()).toAscii()));
    if(organismsIterator == pool->organisms.end())
        return;

    selectedOrganism = &(organismsIterator->second);

    std::set<Reaction*>::iterator reactionsIterator = organismsIterator->second.reactions.begin();
    std::set<Compound*> availableCompounds;

    T_equationMetaboliteIterator equationMetaboliteIterator;

    while(reactionsIterator != organismsIterator->second.reactions.end())
    {
        equationMetaboliteIterator = (*reactionsIterator)->equation->metabolites.begin();

        while(equationMetaboliteIterator != (*reactionsIterator)->equation->metabolites.end())
        {
            availableCompounds.insert(equationMetaboliteIterator->first.compound);
            ++equationMetaboliteIterator;
        }
        ++reactionsIterator;
    }

    endMetabolites.clear();
    std::set<Compound*>::iterator availableCompoundsIterator = availableCompounds.begin();
    while(availableCompoundsIterator != availableCompounds.end())
    {
        ui.listWidget->addItem((*availableCompoundsIterator)->getName().c_str());
        ui.listWidget->item(ui.listWidget->count()-1)->setData(Qt::UserRole,ui.listWidget->count()-1);
        endMetabolites.push_back((*availableCompoundsIterator));
        ++availableCompoundsIterator;
    }

    ui.listWidget->sortItems();

    ui.tab_2->setEnabled(false);
    ui.tab_3->setEnabled(false);
    ui.tab_4->setEnabled(false);

}

void LabelPositionAssistent::on_pushButton_clicked()
{
    if(ui.listWidget->currentRow() == -1)
    {
        QMessageBox::critical(this, "Problem","You need to select a metabolite from the list");
        return;
    }

    // setup next tab


    // calculate all labeled network

    if(metabolicTreeAllLabeled == 0)
        delete metabolicTreeAllLabeled;

    TreeNode startNode;
    startNode.metabolite = myAlgorithms->getOrCreateMetaboliteObject(
        endMetabolites.at(ui.listWidget->currentItem()->data(Qt::UserRole).toInt()),"");
    endCompound = startNode.metabolite->compound;
    
    startNode.metabolite->label.resize(startNode.metabolite->compound->formula.getNumberOfCarbons());
    for(unsigned int n=0;n<startNode.metabolite->label.size();n++)
        startNode.metabolite->label[n] = 1;

    unsigned int transitionCondition = TRANSITIONCONDITION_LABEL | TRANSITIONCONDITION_NOPATH;
    set<Compound*> metaboliteBlackList;

    metabolicTreeAllLabeled = new MetabolicTree(startNode, selectedOrganism->reactions, metaboliteBlackList, transitionCondition, myAlgorithms);


    pool->metabolitesPool->clear();

    QString temp=ui.textEdit_2->toPlainText();
    myAlgorithms->parseReactionUserSettings(string(temp.toAscii()));

    metabolicTreeAllLabeled->setExpandstrategyToReverse(true);
    metabolicTreeAllLabeled->buildReactionTree(0, 0, ui.lineEdit->text().toInt(), false);

    ui.listWidget_2->clear();
    std::map<std::string, MetaboliteObject*>::iterator metabolitesPoolIterator = pool->metabolitesPool->begin();
    std::set<Compound*> startMetabolitesTemp;
    while(metabolitesPoolIterator != pool->metabolitesPool->end())
    {
        startMetabolitesTemp.insert(metabolitesPoolIterator->second->compound);
        ++metabolitesPoolIterator;
    }

    startMetabolites.clear();
    std::set<Compound*>::iterator startMetabolitesTempIterator = startMetabolitesTemp.begin();
    while(startMetabolitesTempIterator != startMetabolitesTemp.end())
    {
        ui.listWidget_2->addItem((*startMetabolitesTempIterator)->getName().c_str());
        ui.listWidget_2->item(ui.listWidget_2->count()-1)->setData(Qt::UserRole,ui.listWidget_2->count()-1);
        startMetabolites.push_back(*startMetabolitesTempIterator);
        ++startMetabolitesTempIterator;
    }

    ui.listWidget_2->sortItems();

    ui.tab_3->setEnabled(true);
    ui.tabWidget->setCurrentIndex(2);

}

void LabelPositionAssistent::on_pushButton_3_clicked()
{
    startCompound = startMetabolites.at(ui.listWidget_2->currentItem()->data(Qt::UserRole).toInt());

    TreeNode tempNode;
    //tempNode.metabolite = myAlgorithms->getOrCreateMetaboliteObject(startCompound,"");

    boost::numeric::ublas::vector<double> label;
    set<Compound*> metaboliteBlackList;
    unsigned int transitionCondition = TRANSITIONCONDITION_LABEL | TRANSITIONCONDITION_NOPATH;

    label.resize(startCompound->formula.getNumberOfCarbons());
    metabolicTrees.clear();
    if(ui.radioButton->isChecked())
    {
        for(unsigned int n=0;n<pow((double)2,startCompound->formula.getNumberOfCarbons());n++)
        {
            if(n==0)// skip if there is no label at all
                continue;
            for(unsigned int m=0;m<startCompound->formula.getNumberOfCarbons();m++)
            {
                //if(m == 0) 
                    //continue;
                if( ((n>>m) & 0x01) == 0x01)
                    label.operator ()(m)= 1;
                else
                    label.operator ()(m)= 0;
            }
            tempNode.metabolite = myAlgorithms->getOrCreateMetaboliteObject(startCompound,"",label);
            metabolicTrees.push_back(MetabolicTree(tempNode,selectedOrganism->reactions, metaboliteBlackList, transitionCondition, myAlgorithms));
        }
    }
    else
    {
        for(unsigned int n=0;n<startCompound->formula.getNumberOfCarbons();n++)
        {
            label.clear();
            label.resize(startCompound->formula.getNumberOfCarbons());
            label.operator ()(n)= 1;
            tempNode.metabolite = myAlgorithms->getOrCreateMetaboliteObject(startCompound,"",label);
            metabolicTrees.push_back(MetabolicTree(tempNode,selectedOrganism->reactions, metaboliteBlackList, transitionCondition, myAlgorithms));
        }
    }

    ui.tableWidget->setRowCount(metabolicTrees.size());
    QTableWidgetItem *twi;
    for(unsigned int n=0;n<metabolicTrees.size();n++)
    {
        twi = new QTableWidgetItem();
        twi->setData(Qt::UserRole, n);
        twi->setText(metabolicTrees.at(n).startNode->metabolite->labelText().c_str());
        ui.tableWidget->setItem(n,0,twi);

        twi = new QTableWidgetItem();
        ui.tableWidget->setItem(n,1,twi);

        twi = new QTableWidgetItem();
        ui.tableWidget->setItem(n,2,twi);

        twi = new QTableWidgetItem();
        ui.tableWidget->setItem(n,3,twi);
    }

    timer.setInterval(100);
    timer.start();
    ui.tabWidget->setCurrentIndex(3);
    ui.tab_4->setEnabled(true);
}

void LabelPositionAssistent::updateNetworksTable(void)
{
    if(ui.tableWidget->rowCount() != metabolicTrees.size())
        return;

    unsigned int row=0;
    T_MetabolicTreesIterator metabolicTreesIterator = metabolicTrees.begin();
    while(metabolicTreesIterator != metabolicTrees.end())
    {
        ui.tableWidget->item(row,2)->setText(QString().setNum(metabolicTreesIterator->numNodes()));

        if(metabolicTreesIterator->endReason == BTE_COMPLETE)
            ui.tableWidget->item(row,3)->setText("completed after " + QString().setNum(metabolicTreesIterator->iterations) + " iterations");
        else if(metabolicTreesIterator->endReason == BTE_MAXITERATIONS)
            ui.tableWidget->item(row,3)->setText("terminated due to max iterations");

        if(metabolicTreesIterator->endReason != BTE_RUNNING)
        {
            MetaboliteObject tempMetabolite;
            tempMetabolite.compound = endCompound;
            tempMetabolite.label.resize(endCompound->formula.getNumberOfCarbons());
            T_treeNodesPoolIterator it = metabolicTreesIterator->treeNodesPool.begin();
            while(it != metabolicTreesIterator->treeNodesPool.end())
            {
                if(it->second.metabolite->compound ==  endCompound)
                {
                    tempMetabolite.label += it->second.metabolite->label;
                }
                ++it;
            }
            ui.tableWidget->item(row,1)->setText(tempMetabolite.labelText().c_str());
        }
        
        ++metabolicTreesIterator;
        ++row;
    }

    ui.tableWidget->resizeColumnsToContents();
}

void LabelPositionAssistent::on_pushButton_5_clicked()
{
    timer.stop();
}

void LabelPositionAssistent::on_pushButton_4_clicked()
{
    MetabolicTree *tree = 0;

    tree = &metabolicTrees.at(
        ui.tableWidget->item(ui.tableWidget->currentRow(),0)->data(Qt::UserRole).toInt());

    if(tree==0)
        return;
    try
    {
        tree->rankedGraph = true;
        tree->createGraph();
    }
    catch(std::exception &e)
    {
        return;
    }

    svgViewer->openFile(QString("C:\\metabolicNavigator.svg"));
    svgViewer->readPlainFile("C:\\graph.dot.plain");
    svgViewer->showMaximized();
}

void LabelPositionAssistent::on_pushButton_6_clicked()
{
    metabolitesViewer->tree= &metabolicTrees.at(
        ui.tableWidget->item(ui.tableWidget->currentRow(),0)->data(Qt::UserRole).toInt());
    metabolitesViewer->updateTable();
    metabolitesViewer->show();
}