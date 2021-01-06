#ifndef BROWSEREACTIONSWIDGET_H
#define BROWSEREACTIONSWIDGET_H

#include <QDockWidget>
#include <vector>
#include <string>
#include "ui_browsereactionswidget.h"

class Pool;
class MyAlgorithms;
class Reaction;
class CompoundTransition;

class BrowseReactionsWidget : public QDockWidget
{
    Q_OBJECT

public:
    BrowseReactionsWidget(QWidget *parent = 0, Pool *pool=0, MyAlgorithms *myAlgorithms=0);
    ~BrowseReactionsWidget();

public slots:
    void showSelectedReactions(void);
    void update(unsigned int signal);


private:
    Ui::BrowseReactionsWidgetClass ui;
    Pool *pool;
    std::vector<std::pair<std::string,Reaction*>> displayReactions;
    MyAlgorithms *myAlgorithms;
    std::vector<CompoundTransition*> currentCompoundTransitions;


private slots:
    void on_tableWidget_itemSelectionChanged();
    void on_pushButton_clicked();
    void on_pushButton_6_clicked();
    void sectionReactionClicked ( int logicalIndex ); 

private:
    void updateReactionDisplay(void);
signals:
    void showDataset(unsigned int);
};

#endif // BROWSEREACTIONSWIDGET_H
