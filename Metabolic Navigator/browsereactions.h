#ifndef BROWSEREACTIONS_H
#define BROWSEREACTIONS_H

#include <QDialog>
#include <QCloseEvent>
#include "ui_browsereactions.h"
#include <map>
#include "Reaction.h"
#include "Options.h"
#include "UserDataset.h"
#include <Vector>
#include "Helper.h"
#include "pool.h"

class BrowseReactions : public QDialog
{
    Q_OBJECT

public:
    BrowseReactions(QWidget *parent = 0, Options *options=0);
    ~BrowseReactions();
    class Options *options;
    class Pool *pool;

    std::vector<std::pair<std::string,Reaction*>> displayReactions;
public slots:
    void showSelected(void);
private:
    Ui::BrowseReactionsClass ui;
    void updateDisplay(void);

private slots:
    void on_tableWidget_itemSelectionChanged();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent*);
    void sectionClicked ( int logicalIndex ); 
    void showReaction(QString rNumber);
signals:
    void displayDataset(unsigned int datasetIndex);
};



#endif // BROWSEREACTIONS_H
