#ifndef METABOLICWALKER_H
#define METABOLICWALKER_H

#include <QDialog>
#include <QScrollArea>
#include <QMenu>
#include "ui_metabolicwalker.h"
#include <map>

#include "boost/shared_ptr.hpp"

class Pool;
class MetaboliteObject;
class Reaction;
class DragWidget;
class MyAlgorithms;
class ConnectionObject;
class MyLabel;

class MetabolicWalker : public QDialog
{
    Q_OBJECT

public:
    MetabolicWalker(QWidget *parent = 0);
    ~MetabolicWalker();
    void createNewMetaboliteObject(MetaboliteObject *metabolite);
    void connectMetabolites(MetaboliteObject *metabolit1, MetaboliteObject *metabolit2, Reaction *reaction);

    QScrollArea *scrollArea;
    DragWidget *mainWidget;
    MyAlgorithms *myAlgorithms;
    QMenu *menuCompound;
    QMenu *menuCompoundCompound;
    QMenu *menuCompoundReaction;
    QMenu *menu;
    QMenu *nameMenu;
    QMenu *menuCompartment;
    QMenu *menuRpairsFilter;
    QAction *actionShowUserDataOnly;
    QAction *actionShowAcceptedOnly;
    QAction *rpairs_off,*rpairs_1,*rpairs_2,*rpairs_3;
    //map<unsigned int, shared_ptr<MetaboliteObject>> metaboliteObjects;
    std::map<unsigned int, boost::shared_ptr<ConnectionObject>> connectionObjects;
    std::map<unsigned int, boost::shared_ptr<ConnectionObject>>::iterator connectionObjectsIterator;
    MyLabel* pCompoundImage;
    unsigned int useRpairs;
    Pool *pool;
    /*
    map<string,MetaboliteObject*> *metabolitesPool;
    set<string> *availableCompartments;
    map<unsigned int, Compound> *compounds; 
    map<unsigned int, Reaction> *reactions; 
    map<string, Compound*> *compoundNames;
    map<unsigned int, UserDataset> *userDatasets;
    */

private:
    Ui::MetabolicWalkerClass ui;
private slots:
    void compoundImageClicked(void);
    void compoundImageRightClick(QPoint pos);
    void mainWidgetRightClick(QPoint pos);
    void mainWidgetLeftClick(QPoint pos);
    void showMetabolitBrowser(void);
    void metaboliteSelected(QString ID);
    void deselectAllFromImage(void);
    void deleteMetabolite(void);
signals:
    void requestMetaboliteFromBrowser(void);
    void addLogLine(QString);
    void deselectAllMetabolites(void);
    void updateConnections(void);
};


#endif // METABOLICWALKER_H
