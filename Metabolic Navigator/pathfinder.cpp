#include "pathfinder.h"
#include "TreeNode.h"
#include "Helper.h"
#include "MetabolicTree.h"
#include "Organism.h"
#include "defines.h"
#include "svgviewer/svgviewer.h"
#include "pool.h"
#include "MetabolicPathViewer.h"
#include "Compound.h"
#include "Reaction.h"
#include "MyAlgorithms.h"
#include "UserDataset.h"
#include "Options.h"
#include <QMenu>
#include "selectlabeldialog.h"


PathFinder::PathFinder(QWidget *parent, Options *options)
    : QDialog(parent)
{
    PathFinder::options = options;
    ui.setupUi(this);
    if(options!=NULL)
    {
        QString temp = options->settings->value("blacklist").toString();
        ui.textEdit_3->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        ui.textEdit_3->insertPlainText(temp);
        temp = options->settings->value("reactionSettings").toString();
        ui.textEdit->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        ui.textEdit->insertPlainText(temp);
        ui.lineEdit->setText(options->settings->value("start metabolite").toString());
        ui.lineEdit_2->setText(options->settings->value("end metabolite").toString());
    }
    startNode = new TreeNode();
    tree = 0;
    svgViewer = new SvgViewer();
    connect(svgViewer, SIGNAL(nodeDoubleClicked(const QString&, QPoint)), this, SLOT(nodeDoubleClicked(const QString&, QPoint)));
    connect(svgViewer, SIGNAL(nodeRightClicked(const QString&, QPoint)), this, SLOT(nodeRightClicked(const QString&, QPoint)));

    menu = new QMenu(svgViewer->area);
    menuChangeName = new QMenu(svgViewer->area);
    menuNeighbourNodes = new QMenu(svgViewer->area);

    ui.groupBox_6->setVisible(false);
    ui.groupBox->setVisible(false);
    ui.checkBox_5->setVisible(false);
    ui.label_10->setVisible(false);
    ui.comboBox_5->setVisible(false);

    ui.comboBox_2->setVisible(false);
    ui.label_9->setVisible(false);

}

PathFinder::~PathFinder()
{
    delete startNode;
    delete svgViewer;
    //delete menuChangeName;
    //delete menu;  // crashes weil es svg viewer gehört
    ui.textEdit_3->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
    ui.textEdit_3->moveCursor(QTextCursor::End,QTextCursor::KeepAnchor);
    QString temp=ui.textEdit_3->textCursor().selectedText();
    options->settings->setValue("blacklist", temp);
    temp=ui.textEdit->toPlainText();
    if(options != 0)
    {
        options->settings->setValue("reactionSettings", temp);
        options->settings->setValue("start metabolite",ui.lineEdit->text());
        options->settings->setValue("end metabolite",ui.lineEdit_2->text());
    }
}

void PathFinder::showCompartmentsInTreeView(void)
{
    ui.treeWidget->clear();

    map<string, string> compartments = selectedOrganism->outsideCompartment;
    map<string, string>::iterator compartmentsIterator;


    bool doneSomething = false;
    QTreeWidgetItem *twi;
    while(!compartments.empty())
    {
        doneSomething = false;
        compartmentsIterator = compartments.begin();

        while(compartmentsIterator != compartments.end())
        {

            // compartments without any outside compartment
            if(compartmentsIterator->second.empty())
            {
                twi = new QTreeWidgetItem();
                twi->setText(0,compartmentsIterator->first.c_str());
                ui.treeWidget->addTopLevelItem(twi);
                compartmentsIterator = compartments.erase(compartmentsIterator);
                doneSomething = true;
                continue;         
            }

            else
            {
                if(ui.treeWidget->topLevelItemCount() == 0 )
                {
                    ++compartmentsIterator;
                    continue;
                }

                bool doneSomething2 = false;
                for(unsigned int n=0;n<ui.treeWidget->topLevelItemCount();n++)
                {
                    twi = ui.treeWidget->topLevelItem(n);
                    if(addToCompartmentTreeView(compartmentsIterator->first, compartmentsIterator->second, twi) == true)
                    {
                        compartmentsIterator = compartments.erase(compartmentsIterator);
                        doneSomething = true;
                        doneSomething2 = true;
                        break;
                    }
                }

                if(!doneSomething2)
                    ++compartmentsIterator;
            }       
        }



        if(!doneSomething)
            break;
    }

}

bool PathFinder::addToCompartmentTreeView(std::string compartment, std::string outside, QTreeWidgetItem* twi)
{
    QTreeWidgetItem* newTwi;
    for(unsigned int n=0;n<twi->columnCount();n++)
    {
        if(string(twi->text(n).toAscii()) == outside)
        {
            newTwi = new QTreeWidgetItem();
            newTwi->setText(0,compartment.c_str());
            twi->addChild(newTwi);
            return true;
        }
    }

    for(unsigned int n=0;n<twi->childCount();n++)
    {
        if(addToCompartmentTreeView(compartment, outside, twi->child(n)))
        {
            return true;
        }
    }
    return false;
}

void PathFinder::updateCompartmentList(void)
{
    set<string>::iterator availableCompartmentsIterator = pool->availableCompartments->begin();
    ui.comboBox_2->clear();
    ui.comboBox_3->clear();
    unsigned int i=0;
    while(availableCompartmentsIterator != pool->availableCompartments->end())
    {
        //action = new QAction(availableCompartmentsIterator->c_str(),this);
        ui.comboBox_2->addItem(availableCompartmentsIterator->c_str());
        ui.comboBox_3->addItem(availableCompartmentsIterator->c_str());
        if(*availableCompartmentsIterator == string("cyto"))
        {
            ui.comboBox_2->setCurrentIndex(i);
            ui.comboBox_3->setCurrentIndex(i);
        }
        availableCompartmentsIterator++;
        i++;
    }
}

list<MetabolicPath<Reaction,MetaboliteObject>> PathFinder::
findPaths(TreeNode *node, MetabolicPath<Reaction, MetaboliteObject> path)
{
    list<MetabolicPath<Reaction,MetaboliteObject>> pathList;
    list<MetabolicPath<Reaction,MetaboliteObject>> pathListTemp;
    list<MetabolicPath<Reaction,MetaboliteObject>>::iterator pathListIterator;

    T_connectedNodesIterator treeNodeConnectionsIterator;
    T_connectedNodes *treeNodeConnections;
    treeNodeConnections = &(node->prevNodes);
    treeNodeConnectionsIterator = treeNodeConnections->begin();

    pathList.clear();
    if(path.length() > maxPathLength)
        return pathList;
    if(treeNodeConnectionsIterator == treeNodeConnections->end())
    {
        if(path.contains(startNode->metabolite))
            pathList.push_back(path);
        return pathList;
    }

    MetabolicPath<Reaction, MetaboliteObject> newPath;

    while(treeNodeConnectionsIterator != treeNodeConnections->end())
    {
        // prevents loops !
        if(path.contains(treeNodeConnectionsIterator->first->metabolite)==true)
        {
            treeNodeConnectionsIterator++;
            continue;
        }
        newPath = path;
        newPath.append(treeNodeConnectionsIterator->second,treeNodeConnectionsIterator->first->metabolite);
        pathListTemp = findPaths(treeNodeConnectionsIterator->first, newPath);
        pathListIterator = pathListTemp.begin();
        while(pathListIterator != pathListTemp.end())
        {
            pathList.push_back(*pathListIterator);
            pathListIterator++;
        }
        treeNodeConnectionsIterator++;
    }
    return pathList;
}

void PathFinder::on_pushButton_clicked()
{
    ui.pushButton->setEnabled(false);
    unsigned int maxSteps=0;
    endNode=0;

    on_pushButton_6_clicked();

    set<Reaction*> reactionSet;
    maxSteps = atoi(ui.lineEdit_3->text().toLatin1());
    
    T_organismsIterator organismsIterator = pool->organisms.find(string(ui.comboBox_4->itemText(ui.comboBox_4->currentIndex()).toAscii()));
    if(organismsIterator == pool->organisms.end())
    {
        ui.pushButton->setEnabled(false);
        return;
    }

    reactionSet = organismsIterator->second.reactions;


    bool unique=true;
    set<Compound*> foundCompounds;
    myAlgorithms->findCompounds(QStringList(ui.lineEdit->text()),foundCompounds, unique);
    if(!unique)
    {
        emit addLogLine("Start metabolite name is not unique!");
        ui.pushButton->setEnabled(true);
        return;
    }

    pool->metabolitesPool->clear(); // memory leak!!!

    string startMetaboliteCompartment;
    startMetaboliteCompartment = ui.comboBox_2->currentText().toLatin1();
    if(startMetaboliteCompartment == "none")
        startMetaboliteCompartment = "";

    //startNode->metabolite = myAlgorithms->getOrCreateMetaboliteObject(&(*pool->compounds)[(*pool->selectedCompounds)[0]],startMetaboliteCompartment);
    set<string>::iterator selectedCompoundsIterator = pool->selectedCompounds->begin();
    startNode->metabolite = myAlgorithms->getOrCreateMetaboliteObject(&(*pool->compounds)[*selectedCompoundsIterator],startMetaboliteCompartment);
    startNode->metabolite->label = startLabel;

    set<Compound*> metaboliteBlackList;

    QString temp = ui.textEdit_3->toPlainText();

    map<unsigned int, Compound*> blacklistCompounds;

    /*
    bool unique=true;
    myAlgorithms->findCompounds(temp.split('\n'),&blacklistCompounds,&unique);
    if(!unique)
    {
        ui.pushButton->setEnabled(true);
        return;
    }
    

    for(unsigned int n=0;n<blacklistCompounds.size();n++)
        metaboliteBlackList.insert(blacklistCompounds[n]);
    */

    unique=true;
    myAlgorithms->findCompounds(temp.split('\n'),metaboliteBlackList,unique);
    if(!unique)
    {
        ui.pushButton->setEnabled(true);
        return;
    }


    MetaboliteObject *endMetabolite = new MetaboliteObject();
    unique=true;
    foundCompounds.clear();
    myAlgorithms->findCompounds(QStringList(ui.lineEdit_2->text()),foundCompounds,unique);
    if(!unique)
    {
        emit addLogLine("End metabolite name is not unique!");
        ui.pushButton->setEnabled(true);
        return;
    }


    string endMetaboliteCompartment;
    endMetaboliteCompartment = ui.comboBox_3->currentText().toLatin1();
    if(endMetaboliteCompartment == "none")
        endMetaboliteCompartment = "";
    selectedCompoundsIterator = pool->selectedCompounds->begin();
    //endMetabolite = myAlgorithms->getOrCreateMetaboliteObject(&(*pool->compounds)[(*pool->selectedCompounds)[0]],endMetaboliteCompartment);
    endMetabolite = myAlgorithms->getOrCreateMetaboliteObject(&(*pool->compounds)[*selectedCompoundsIterator],endMetaboliteCompartment);

    unsigned int transitionCondition_match,transitionCondition_mismatch;
    unsigned int transitionCondition = 0;
    transitionCondition_match = ui.comboBox->currentIndex();
    transitionCondition_mismatch = ui.comboBox_5->currentIndex();

    switch(transitionCondition_match)
    {
    case 0:
        transitionCondition = TRANSITIONCONDITION_ANY;
        break;
    case 1:
        transitionCondition = TRANSITIONCONDITION_MAIN;
        break;
    case 2:
        transitionCondition = TRANSITIONCONDITION_CARBON;
        startNode->metabolite->label.clear();
        break;
    case 3:
        transitionCondition = TRANSITIONCONDITION_LABEL;
        break;
    };

   
    switch(transitionCondition_mismatch)
    {
    case 0:
        transitionCondition |= TRANSITIONCONDITION_ANYPATH;
        break;
    case 1:
        transitionCondition |= TRANSITIONCONDITION_NOPATH;
        break;
    };
    

    if(tree!=0)
        delete tree;
    tree = new MetabolicTree(*startNode, reactionSet, metaboliteBlackList, transitionCondition, myAlgorithms);
    tree->buildReactionTree(endMetabolite, &endNode, maxSteps, ui.checkBox_5->isChecked());

    emit addLogLine("Tree has "+QString().setNum(tree->numNodes())+ " nodes and "+QString().setNum(tree->numEdges()) + " connections");
    if(tree->endReason == BTE_COMPLETE)
        emit addLogLine("tree is complete");
    if(tree->endReason == BTE_MAXITERATIONS)
        emit addLogLine("quit due to max iterations limitation");


    if(endNode==0)
        ui.pushButton_2->setEnabled(false);
    else
    {
        ui.pushButton_2->setEnabled(true);
        char dist[10];
        _itoa_s(endNode->distance,&dist[0],10,10);
        ui.lineEdit_4->setText(dist);
    }
    ui.pushButton->setEnabled(true);

    if(startNode->metabolite == 0)
    {
        ui.pushButton_4->setEnabled(false);
    }
    else
    {
        ui.pushButton_4->setEnabled(true);
    }
}

void PathFinder::on_pushButton_2_clicked()
{
    list<MetabolicPath<Reaction,MetaboliteObject>> metabolicPathList;
    list<MetabolicPath<Reaction,MetaboliteObject>>::iterator metabolicPathListIterator;

    if(endNode==0)
        return;

    metaboliteKeysLevel.clear();    
    MetabolicPath<Reaction,MetaboliteObject> startPath;
    startPath.setEndMetabolite(endNode->metabolite);
    maxPathLength = atoi(ui.lineEdit_4->text().toLatin1());
    metabolicPathList=findPaths(endNode,startPath);

    emit addLogLine("findPaths: " + QString().setNum(metabolicPathList.size())+ " paths found");

    metabolicPathViewer->pathways.clear();
    metabolicPathListIterator = metabolicPathList.begin();
    for(unsigned int n=0;n<metabolicPathList.size();n++)
    {
        metabolicPathViewer->pathways.push_back(pair<MetabolicPath<Reaction,MetaboliteObject>, unsigned int>
            (*metabolicPathListIterator,metabolicPathListIterator->length()));
        /*
        emit addLogLine("Path nr. "+ QString().setNum(n+1));
        string textOut;
        metabolicPathListIterator->generateTextOutput(&textOut);
        emit addLogLine(textOut.c_str());
        */
        metabolicPathListIterator++;
    }
    metabolicPathViewer->updateAvailablePaths();
}

void PathFinder::on_pushButton_3_clicked()
{
    metabolicPathViewer->show();
}

void PathFinder::on_pushButton_4_clicked()
{
    if(tree==0)
        return;
    try
    {
        if(ui.treeWidget->topLevelItemCount()!=0)
            tree->setSubgraphHierachy(ui.treeWidget->topLevelItem(0));
        else
            tree->setSubgraphHierachy(0);

        tree->rankedGraph = ui.checkBox_RankedGraph->isChecked();
        tree->createGraph();
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in createGraph: ");
        emit addLogLine(e.what());
        return;
    }

    svgViewer->openFile(QString("C:\\metabolicNavigator.svg"));
    svgViewer->readPlainFile("C:\\graph.dot.plain");
    svgViewer->showMaximized();
}

void PathFinder::nodeRightClicked(const QString& nodeId,  QPoint pos)
{

    QAction *action;

    menu->clear();
    menuChangeName->clear();
    menuNeighbourNodes->clear();
    
    menuChangeName->setTitle("Change name");
    menuNeighbourNodes->setTitle("Connected metabolites");
    TreeNode *treeNode;
    tree->getTreeNodeFromId(string(nodeId.toAscii()), treeNode);

    if(treeNode == 0)
        return;

    if(treeNode->metabolite->compound!=0)
    {
        std::vector<std::string>::iterator namesIterator;
        namesIterator = treeNode->metabolite->compound->names.begin();
        while(namesIterator != treeNode->metabolite->compound->names.end())
        {
            action = new QAction(namesIterator->c_str(),this);
            action->setData(1);
            menuChangeName->addAction(action);
            ++namesIterator;
        }
    }

    QMenu *subMenu;

    string tempString;
    set<std::string> addedMetabolites;
    vector<Reaction*>::iterator connectedNodesReactionsIterator;
    T_connectedNodesIterator connectedNodesIterator = treeNode->nextNodes.begin();
    while(connectedNodesIterator != treeNode->nextNodes.end())
    {
        if(addedMetabolites.find(connectedNodesIterator->first->metabolite->key()) == addedMetabolites.end())
        {
            subMenu = new QMenu();
            subMenu->setTitle(connectedNodesIterator->first->metabolite->text(pool).c_str());
            menuNeighbourNodes->addMenu(subMenu);

            connectedNodesReactionsIterator = connectedNodesIterator->second.begin();
            while(connectedNodesReactionsIterator != connectedNodesIterator->second.end())
            {
                action = new QAction(0);
                tempString =  (*connectedNodesReactionsIterator)->id + " : " + (*connectedNodesReactionsIterator)->equation->text(false,true,pool);
                action->setText(tempString.c_str());
                subMenu->addAction(action);
                ++connectedNodesReactionsIterator;
            }
            addedMetabolites.insert(connectedNodesIterator->first->metabolite->key());
        }
        ++connectedNodesIterator;
    }
    connectedNodesIterator = treeNode->prevNodes.begin();
    while(connectedNodesIterator != treeNode->prevNodes.end())
    {
        if(addedMetabolites.find(connectedNodesIterator->first->metabolite->key()) == addedMetabolites.end())
        {
            subMenu = new QMenu();
            subMenu->setTitle(connectedNodesIterator->first->metabolite->text(pool).c_str());
            menuNeighbourNodes->addMenu(subMenu);

            connectedNodesReactionsIterator = connectedNodesIterator->second.begin();
            while(connectedNodesReactionsIterator != connectedNodesIterator->second.end())
            {
                action = new QAction(0);
                tempString =  (*connectedNodesReactionsIterator)->id + " : " + (*connectedNodesReactionsIterator)->equation->text(false,true,pool);
                action->setText(tempString.c_str());
                subMenu->addAction(action);
                ++connectedNodesReactionsIterator;
            }
            addedMetabolites.insert(connectedNodesIterator->first->metabolite->key());
        }
        ++connectedNodesIterator;
    }

    menu->addAction("Expand");
    menu->addAction("Delete");
    menu->addMenu(menuChangeName);
    menu->addSeparator();
    menu->addMenu(menuNeighbourNodes);

    action = menu->exec(pos);

    if(action == 0)
        return;

    if(action->text() == "Delete")
    {
        try
        {
            tree->deleteNode(string(nodeId.toAscii()));
        }
        catch(std::exception &e)
        {
            emit addLogLine("Error in deleteNode: ");
            emit addLogLine(e.what());
            return;
        }

    }
    else if(action->text() == "Expand")
    {
        try
        {
            tree->expandNode(string(nodeId.toAscii()));
        }
        catch(std::exception &e)
        {
            emit addLogLine("Error in expandNode: ");
            emit addLogLine(e.what());
            return;
        }
    }
    if(action->data().toInt() == 1)
    {
        treeNode->metabolite->displayText = string(action->text().toAscii());
    }


    if(tree==0)
        return;
    try
    {
        tree->createGraph();
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in createGraph: ");
        emit addLogLine(e.what());
        return;
    }

    try
    {
        svgViewer->saveZoomAndPosition();
        svgViewer->openFile(QString("C:\\metabolicNavigator.svg"));
        svgViewer->readPlainFile("C:\\graph.dot.plain");
        svgViewer->restoreZoomAndPosition();
        //svgViewer->setFocus();
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in svgView: ");
        emit addLogLine(e.what());
        return;
    }

}

void PathFinder::nodeDoubleClicked(const QString& nodeId,  QPoint pos)
{

    try
    {
        tree->expandNode(string(nodeId.toAscii())); 
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in expandNode: ");
        emit addLogLine(e.what());
        return;
    }



    if(tree==0)
        return;
    try
    {
        tree->createGraph();
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in createGraph: ");
        emit addLogLine(e.what());
        return;
    }

    try
    {
        svgViewer->saveZoomAndPosition();
        svgViewer->openFile(QString("C:\\metabolicNavigator.svg"));
        svgViewer->readPlainFile("C:\\graph.dot.plain");
        svgViewer->restoreZoomAndPosition();
        //svgViewer->setFocus();
    }
    catch(std::exception &e)
    {
        emit addLogLine("Error in svgView: ");
        emit addLogLine(e.what());
        return;
    }
}



void PathFinder::on_comboBox_2_editTextChanged(QString text)
{
}

void PathFinder::on_comboBox_2_currentIndexChanged(QString text)
{
    if(text == "none")
    {
        ui.comboBox_3->setCurrentIndex(ui.comboBox_2->currentIndex());
    }
}

void PathFinder::update(unsigned int signal)
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
    else if(signal == UPDATE_AVAILABLE_COMPARTMENTS)
    {
        updateCompartmentList();
    }
}

void PathFinder::on_comboBox_4_currentIndexChanged(int itemIndex)
{
    T_organismsIterator organismsIterator = pool->organisms.find(string(ui.comboBox_4->itemText(itemIndex).toAscii()));
    if(organismsIterator == pool->organisms.end())
        return;
    selectedOrganism = &organismsIterator->second;
    showCompartmentsInTreeView();
}

void PathFinder::on_pushButton_5_clicked()
{
    set<Reaction*> reactionSet;
    
    T_organismsIterator organismsIterator = pool->organisms.find(string(ui.comboBox_4->itemText(ui.comboBox_4->currentIndex()).toAscii()));
    if(organismsIterator == pool->organisms.end())
        return;

    reactionSet = organismsIterator->second.reactions;


    bool unique=true;
    set<Compound*> foundCompounds;
    set<Compound*>::iterator foundCompoundsIterator;
    myAlgorithms->findCompounds(QStringList(ui.lineEdit->text()),foundCompounds, unique);
    if(!unique)
    {
        emit addLogLine("Start metabolite name is not unique!");
        ui.pushButton->setEnabled(true);
        return;
    }
    foundCompoundsIterator = foundCompounds.begin();

    MetaboliteObject *metabolite;
    metabolite = myAlgorithms->getOrCreateMetaboliteObject(*foundCompoundsIterator,"none");
    if(metabolite->label.size()==0)
    {
        metabolite->label.resize(metabolite->compound->formula.getNumberOfCarbons(),false);
        metabolite->label.clear();
    }

    SelectLabelDialog selectLabelDialog;
    selectLabelDialog.setModal(true);
    selectLabelDialog.setVector(metabolite->label);
    selectLabelDialog.setWindowTitle("Select labels for " + QString(metabolite->compound->getName().c_str()));
    selectLabelDialog.exec();
    metabolite->label = selectLabelDialog.getVector();
    startLabel = metabolite->label;
    ui.lineEdit_5->setText(metabolite->labelText().c_str());
}

void PathFinder::on_pushButton_6_clicked()
{
    QString temp=ui.textEdit->toPlainText();
 
    myAlgorithms->parseReactionUserSettings(string(temp.toAscii()));
}