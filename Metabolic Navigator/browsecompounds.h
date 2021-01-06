#ifndef BROWSECOMPOUNDS_H
#define BROWSECOMPOUNDS_H

#include <QDialog>
//#include "ui_browsecompounds.h"
#include <map>
#include "Compound.h"
#include "Reaction.h"
#include "Options.h"
#include <QCloseEvent>
#include "pool.h"
#include "myAlgorithms.h"

class BrowseCompounds : public QDialog
{
    Q_OBJECT

public:
    BrowseCompounds(QWidget *parent = 0, Options *options=0);
    ~BrowseCompounds();
    void updateDisplay(void);

    class Options *options;
    class Pool *pool;
    class MyAlgorithms *myAlgorithms;

    std::vector<std::pair<std::string,Compound*>> displayCompounds;
public slots:
    void showSelected(void);

private:
    Ui::BrowseCompoundsClass ui;

private slots:
    void on_listWidget_2_itemDoubleClicked(QListWidgetItem*);
    void on_tableWidget_cellDoubleClicked(int,int);
    void on_tableWidget_itemSelectionChanged();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent *event);
signals:
    void metaboliteSelected(QString metaboliteID);
    void showReaction(QString rNumber);
};

#endif // BROWSECOMPOUNDS_H
