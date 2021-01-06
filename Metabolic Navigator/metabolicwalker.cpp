#include "metabolicwalker.h"
#include <QLayout>
#include <QPixmap>
#include "Helper.h"
#include "MyAlgorithms.h"
#include "pool.h"
#include "Compound.h"
#include "Reaction.h"
#include "UserDataset.h"
#include "ConnectionObject.h"
#include "MetaboliteObject.h"
#include "dragwidget.h"
#include "mylabel.h"


using namespace std;
using namespace boost;

MetabolicWalker::MetabolicWalker(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout); 
    
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable( true );
    scrollArea->setBackgroundRole(QPalette::Dark);
    layout->addWidget(scrollArea);

    mainWidget = new DragWidget(scrollArea);
    mainWidget->connectionObjects = &connectionObjects;
    mainWidget->move(0,0);
    scrollArea->setGeometry(QRect(0,0,200,200));
    scrollArea->setWidget(mainWidget);
    menuCompound = new QMenu(mainWidget);
    menu = new QMenu(mainWidget);
    menuCompoundCompound = new QMenu("Metabolites",mainWidget);
    menuCompoundReaction = new QMenu("Reactions",mainWidget);
    nameMenu = new QMenu("Alternative names");
    menuCompartment = new QMenu("Select compartment");

    menuCompound->addMenu(nameMenu);
    menuCompound->addMenu(menuCompoundCompound);
    menuCompound->addMenu(menuCompoundReaction);
    menuCompound->addMenu(menuCompartment);
    connect(mainWidget, SIGNAL(rightClick(QPoint)), this, SLOT(mainWidgetRightClick(QPoint)));
    connect(mainWidget, SIGNAL(leftClick(QPoint)), this, SLOT(mainWidgetLeftClick(QPoint)));
    connect(this, SIGNAL(updateConnections()), mainWidget, SLOT(updateConnections()));

    QAction actionShowMetabolitBrowser("Show metabolite browser",this);    
    connect(&actionShowMetabolitBrowser,SIGNAL(triggered()),this,SLOT(showMetabolitBrowser()));
    menu->addAction(&actionShowMetabolitBrowser);

    actionShowUserDataOnly = new QAction("Show user data only",this);
    actionShowUserDataOnly->setCheckable(true);
    menu->addAction(actionShowUserDataOnly);

    actionShowAcceptedOnly = new QAction("Show accepted only",this);
    actionShowAcceptedOnly->setCheckable(true);
    menu->addAction(actionShowAcceptedOnly);

    menuRpairsFilter = new QMenu("Use rpairs filter",this);
    menu->addMenu(menuRpairsFilter);

    rpairs_off = new QAction("off",this);
    rpairs_off->setData(0);
    rpairs_off->setCheckable(true);
    menuRpairsFilter->addAction(rpairs_off);

    rpairs_1 = new QAction("level 1",this);
    rpairs_1->setData(1);
    rpairs_1->setCheckable(true);
    menuRpairsFilter->addAction(rpairs_1);
    rpairs_2 = new QAction("level 2",this);
    rpairs_2->setCheckable(true);
    rpairs_2->setData(2);
    menuRpairsFilter->addAction(rpairs_2);
    rpairs_3 = new QAction("level 3",this);
    rpairs_3->setCheckable(true);
    rpairs_3->setData(3);
    menuRpairsFilter->addAction(rpairs_3);

    useRpairs=0;
    rpairs_off->setChecked(true);
    
    actionShowUserDataOnly->setChecked(true);

}

MetabolicWalker::~MetabolicWalker()
{
    delete mainWidget;
    delete scrollArea;
    delete nameMenu;
    delete menuCompartment;
    delete actionShowUserDataOnly;
    delete actionShowAcceptedOnly;
}
    
void MetabolicWalker::metaboliteSelected(QString ID)
{
    string id = ID.toLatin1();
    MetaboliteObject* metaboliteObject = myAlgorithms->getOrCreateMetaboliteObject(&(*pool->compounds)[id],"none");
    createNewMetaboliteObject(metaboliteObject);
}

void MetabolicWalker::showMetabolitBrowser(void)
{
    emit requestMetaboliteFromBrowser();
}

void MetabolicWalker::mainWidgetRightClick(QPoint pos)
{
    QAction *action = menu->exec(pos);
    if(action == actionShowUserDataOnly && actionShowUserDataOnly->isChecked()==false)
        actionShowAcceptedOnly->setEnabled(false);
    else if(action == actionShowUserDataOnly && actionShowUserDataOnly->isChecked()==true)
        actionShowAcceptedOnly->setEnabled(true);
    else if(action == rpairs_off || action == rpairs_1 || action == rpairs_2 || action==rpairs_3)
    {
        rpairs_off->setChecked(false);
        rpairs_1->setChecked(false);
        rpairs_2->setChecked(false);
        rpairs_3->setChecked(false);
        action->setChecked(true);
        useRpairs = action->data().toInt();
    }
}

void MetabolicWalker::mainWidgetLeftClick(QPoint pos)
{
    emit deselectAllMetabolites();
}

void MetabolicWalker::deselectAllFromImage(void)
{
    emit deselectAllMetabolites();
}

void MetabolicWalker::compoundImageRightClick(QPoint pos)
{
    set<Reaction*> reactionSet;

    map<MetaboliteObject*, vector<Reaction *>> targetMetaboliteLinkedReactions;
    map<Reaction *, vector<MetaboliteObject *>> targetReactionLinkedMetabolites;

    map<MetaboliteObject*, vector<Reaction *>>::iterator targetMetaboliteLinkedReactionsIterator;
    map<Reaction *, vector<MetaboliteObject*>>::iterator targetReactionLinkedMetabolitesIterator;

    map<string,Compound *> compoundNames;
    map<string,Compound *>::iterator compoundNamesIterator;
    map<string, Compound>::iterator compoundsIterator;
    compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        compoundNames[compoundsIterator->second.cNumber]=&compoundsIterator->second;
        compoundsIterator++;
    }
    /*
    for(unsigned int n=0;n<pool->compounds->size();n++)
        compoundNames[(*pool->compounds)[n].cNumber]=&(*pool->compounds)[n];
    */

    MyLabel * senderImage = static_cast<MyLabel*>(sender());
    MetaboliteObject *senderMetabolite = senderImage->metaboliteObject;
    //Compound *senderCompound = metabolite->compound;

    QAction *action;
    QList<QVariant> list;

    nameMenu->clear();
    vector<string>::iterator namesIterator;
    namesIterator = senderMetabolite->compound->names.begin();
    while(namesIterator != senderMetabolite->compound->names.end())
    {
        nameMenu->addAction(namesIterator->c_str());
        ++namesIterator;
    }
    /*
    for(unsigned int n=0;n<senderMetabolite->compound->names.size();n++)
    {
        nameMenu->addAction(senderMetabolite->compound->names[n].c_str());
    }
    */

    set<string>::iterator availableCompartmentsIterator = pool->availableCompartments->begin();
    menuCompartment->clear();
    while(availableCompartmentsIterator != pool->availableCompartments->end())
    {
        action = new QAction(availableCompartmentsIterator->c_str(),this);
        action->setCheckable(true);
        if(availableCompartmentsIterator->c_str() == senderMetabolite->compartment)
            action->setChecked(true);
        action->setData(QString("compartment"));
        menuCompartment->addAction(action);
        availableCompartmentsIterator++;
    }
    
    T_reactionsIterator reactionsIterator;
    
    reactionsIterator = pool->reactions->begin();
    bool onlyUser = actionShowUserDataOnly->isChecked();
    bool onlyAccepted = actionShowAcceptedOnly->isChecked();
    for(unsigned int n=0;n<pool->reactions->size();n++)
    {
        if(onlyUser)
        {
            if(reactionsIterator->second.dataset == -1)
            {
                reactionsIterator++;
                continue;
            }
            if(onlyAccepted==true)
            {
                if((*pool->userDatasets)[reactionsIterator->second.dataset].accepted==false)
                {
                    reactionsIterator++;
                    continue;
                }
            }
        }
        reactionSet.insert(&(reactionsIterator->second));
        reactionsIterator++;
    }

    bool withCompartments = false;
    if(senderMetabolite->compartment != "none")
        withCompartments=true;
    myAlgorithms->calculatePossibleNewMetabolites(senderMetabolite,&reactionSet,&targetMetaboliteLinkedReactions,
        &targetReactionLinkedMetabolites, withCompartments, useRpairs);
    
    QMenu *subMenu;
    QAction  *UserKeggSeparator = menuCompoundReaction->addSeparator();
    
    QPixmap pm(8,8);
    QPainter painter(&pm);
    painter.fillRect(QRect(1,1,7,7),Qt::green);
    painter.end();
    QIcon iconAccepted(pm);


    Reaction *currentReaction;
    //Compound *currentCompound;
    MetaboliteObject *currentMetabolite;



    menuCompoundReaction->clear();
    targetReactionLinkedMetabolitesIterator = targetReactionLinkedMetabolites.begin();
    for(unsigned int n=0;n<targetReactionLinkedMetabolites.size();n++)
    {
        currentReaction = targetReactionLinkedMetabolitesIterator->first;
        subMenu = new QMenu(( targetReactionLinkedMetabolitesIterator->first->id + " " +
            targetReactionLinkedMetabolitesIterator->first->names.begin()->c_str()).c_str());
        menuCompoundReaction->addMenu(subMenu);
        UserKeggSeparator = subMenu->addSeparator();
        for(unsigned int k=0;k<targetReactionLinkedMetabolitesIterator->second.size();k++)
        {
            currentMetabolite = targetReactionLinkedMetabolitesIterator->second[k];
            //action = new QAction(( currentMetabolite->compound->cNumber + " " +
              //  currentMetabolite->compound->names[0]).c_str(),this);
            action = new QAction(currentMetabolite->text(pool, false, true).c_str(),this);
            list.clear();
            list.push_back(QVariant((long long)currentReaction));
            list.push_back(QVariant((long long)currentMetabolite));
            action->setData(list);
            subMenu->addAction(action);
        }
        targetReactionLinkedMetabolitesIterator++;
    }


    menuCompoundCompound->clear();
    targetMetaboliteLinkedReactionsIterator = targetMetaboliteLinkedReactions.begin();
    for(unsigned int n=0;n<targetMetaboliteLinkedReactions.size();n++)
    {
        //subMenu = new QMenu((targetMetaboliteLinkedReactionsIterator->first->compound->cNumber + " " +
            //targetMetaboliteLinkedReactionsIterator->first->compound->names[0]).c_str());
        subMenu = new QMenu(targetMetaboliteLinkedReactionsIterator->first->text(pool,false, true).c_str());
        menuCompoundCompound->addMenu(subMenu);
        UserKeggSeparator = subMenu->addSeparator();
        for(unsigned int k=0;k<targetMetaboliteLinkedReactionsIterator->second.size();k++)
        {
            currentReaction = targetMetaboliteLinkedReactionsIterator->second[k];
            if(currentReaction->dataset != -1)
            {
                action = new QAction(( currentReaction->id + " " +
                    //currentReaction->equation->textWithNames(pool)).c_str(),this);
                    currentReaction->equation->text(false, true, pool)).c_str(),this);
                if((*pool->userDatasets)[currentReaction->dataset].accepted==true)
                {
                    action->setIcon(iconAccepted);
                    subMenu->insertAction(UserKeggSeparator,action);
                }
                else if(actionShowAcceptedOnly->isChecked()==false)
                    subMenu->insertAction(UserKeggSeparator,action);
                else
                    continue;
            }
            else if(actionShowUserDataOnly->isChecked()==false)
            {
                action = new QAction(( currentReaction->id + " " +
                    currentReaction->names.begin()->c_str()).c_str(),this);
                subMenu->addAction(action);
            }
            else
                continue;
            list.clear();
            list.push_back(QVariant((long long)targetMetaboliteLinkedReactionsIterator->second[k]));
            list.push_back(QVariant((long long)targetMetaboliteLinkedReactionsIterator->first));
            action->setData(list);
            //action->setToolTip(currentReaction->equation->textWithNames(pool).c_str());
            action->setToolTip(currentReaction->equation->text(true, true, pool).c_str());
        }
        targetMetaboliteLinkedReactionsIterator++;
    }

    action = menuCompound->exec(pos);
    if(action != 0)
    {
        list = action->data().toList();
        if(action->data().canConvert(QVariant::String)==true)
        {
            senderMetabolite->compartment = action->text().toLatin1(); 
            QString name;
            name = senderMetabolite->text(pool, true, true).c_str();
            senderImage->setText(name);
        }
        else
        {

            currentReaction = (Reaction*)list[0].value<long long>();
            currentMetabolite = (MetaboliteObject*)list[1].value<long long>();
            emit addLogLine(QString("adding path via ") + currentReaction->id.c_str() + " to " + currentMetabolite->compound->cNumber.c_str());
            
            if(withCompartments==false)
                currentMetabolite->compartment="none";
            createNewMetaboliteObject(currentMetabolite);
            connectMetabolites(senderMetabolite, currentMetabolite, currentReaction);
        }
    }
}

void MetabolicWalker::connectMetabolites(MetaboliteObject *metabolit1, MetaboliteObject  *metabolit2, Reaction *reaction)
{
    boost::shared_ptr<ConnectionObject> connectionObject = (boost::shared_ptr<ConnectionObject>)new ConnectionObject();
    connectionObject->leftMetabolite = metabolit1;
    connectionObject->rightMetabolite = metabolit2;
    connectionObject->reaction = reaction;
    connectionObjects[connectionObjects.size()] = connectionObject;
    emit updateConnections();
}


void MetabolicWalker::compoundImageClicked(void)
{
}

void MetabolicWalker::deleteMetabolite(void)
{
    MetaboliteObject *metabolite = static_cast<MyLabel*>(sender())->metaboliteObject;
    connectionObjectsIterator = connectionObjects.begin();
    map<unsigned int, boost::shared_ptr<ConnectionObject>>::iterator connectionObjectsIterator2;
    while(connectionObjectsIterator != connectionObjects.end())
    {
        if(connectionObjectsIterator->second->leftMetabolite == metabolite || connectionObjectsIterator->second->rightMetabolite == metabolite)
        {
            connectionObjectsIterator2 = connectionObjectsIterator;
            connectionObjectsIterator2++;
            //delete connectionObjectsIterator->second;
            connectionObjects.erase(connectionObjectsIterator);
            connectionObjectsIterator = connectionObjectsIterator2;
        }
        else
            connectionObjectsIterator++;
    }
    disconnect(metabolite->imageObject,0,this,0);
    disconnect(this,0,metabolite->imageObject,0);
    metabolite->imageObject->close();
    delete metabolite->imageObject;
}

void MetabolicWalker::createNewMetaboliteObject(MetaboliteObject *metaboliteObject)
{
    QString name;
    name = metaboliteObject->text(pool, true, true).c_str();
    //name = metaboliteObject->compound->names[0].c_str();

    metaboliteObject->imageObject = new MyLabel(name,mainWidget);
    
    metaboliteObject->imageObject->setToolTip(metaboliteObject->compound->cNumber.c_str());
    metaboliteObject->imageObject->move(0,0);
    metaboliteObject->imageObject->show();
    metaboliteObject->imageObject->metaboliteObject = metaboliteObject;
    connect(metaboliteObject->imageObject,SIGNAL(rightClick(QPoint)),this,SLOT(compoundImageRightClick(QPoint)));
    connect(this,SIGNAL(deselectAllMetabolites()),metaboliteObject->imageObject,SLOT(deselect()));
    connect(metaboliteObject->imageObject,SIGNAL(deselectAll()),this,SLOT(deselectAllFromImage()));
    connect(metaboliteObject->imageObject,SIGNAL(deleteMetabolite()),this,SLOT(deleteMetabolite()));
}