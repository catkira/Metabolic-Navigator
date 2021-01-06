#ifndef BROWSEDIALOG_H
#define BROWSEDIALOG_H

#include <QDialog>
#include "ui_browsedialog.h"
#include <vector>
#include <string>

#include "Helper.h"
#include "transitionview.h"

class Pool;
class Options;

class BrowseDialog : public QDialog
{
    Q_OBJECT

public:
    BrowseDialog(QWidget *parent = 0, Options *options=0, Pool *pool=0, MyAlgorithms *myAlgorithms = 0);
    ~BrowseDialog();
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    Options *options;
    TransitionView transitionView;

    std::vector<std::pair<std::string,Compound*>> displayCompounds;
    std::vector<CompoundTransition*> displayedCompoundTransitions;
    std::vector<std::pair<std::string,Reaction*>> displayReactions;

public slots:
    void showSelectedCompounds(void);
    void showSelectedReactions(void);

private:
    void updateCompoundDisplay(void);
    void updateReactionDisplay(void);
    Ui::browseDialogClass ui;

private slots:
    void on_listWidget_3_currentRowChanged(int);
    void on_pushButton_4_clicked();
    void on_tableWidget_2_itemSelectionChanged();
    void on_pushButton_6_clicked();
    void on_pushButton_clicked();
    void on_tableWidget_cellDoubleClicked(int,int);
    void on_tableWidget_itemSelectionChanged();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void closeEvent(QCloseEvent *event);
    void sectionReactionClicked ( int logicalIndex ); 
    void sectionCompoundClicked ( int logicalIndex ); 
signals:
    void compoundSelected(QString compoundID);
    void showReaction(QString rNumber);
    void showDataset(unsigned int);
};

#endif // BROWSEDIALOG_H
