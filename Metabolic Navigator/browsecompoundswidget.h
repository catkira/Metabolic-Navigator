#ifndef BROWSECOMPOUNDSWIDGET_H
#define BROWSECOMPOUNDSWIDGET_H

#include <QDockWidget>
#include <map>
#include <vector>
#include <string>
#include "ui_browsecompoundswidget.h"
#include "TransitionView.h"

class Compound;
class Pool;
class MyAlgorithms;
class CompoundTransition;

class BrowseCompoundsWidget : public QDockWidget
{
    Q_OBJECT

public:
    BrowseCompoundsWidget(QWidget *parent = 0, Pool *pool=0, MyAlgorithms* myAlgorithms = 0);
    ~BrowseCompoundsWidget();

    TransitionView transitionView;

    std::vector<std::pair<std::string,Compound*>> displayCompounds;
    std::vector<CompoundTransition*> displayedCompoundTransitions;


public slots:
    void showSelectedCompounds(void);
    void closeEvent ( QCloseEvent * event );
    void update(unsigned int signal);

private:
    void updateCompoundDisplay(void);
    Ui::BrowseCompoundsWidgetClass ui;
    Pool* pool;
    MyAlgorithms* myAlgorithms;
    std::vector<CompoundTransition*> currentCompoundTransitions;

private slots:

    void on_listWidget_3_itemSelectionChanged();
    void on_tableWidget_itemSelectionChanged();
    void on_tableWidget_cellDoubleClicked(int,int);
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
signals:
    void compoundSelected(QString compoundID);
};

#endif // BROWSECOMPOUNDSWIDGET_H
