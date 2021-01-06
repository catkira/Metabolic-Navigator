#ifndef BROWSEDATASETS_H
#define BROWSEDATASETS_H

#include <QDialog>
#include "ui_browsedatasets.h"
#include <QCloseEvent>

class Options;

class BrowseDatasets : public QDialog
{
    Q_OBJECT

public:
    BrowseDatasets(QWidget *parent = 0, Options *options=0);
    ~BrowseDatasets();
    std::vector<std::pair<std::string,unsigned int>> displayDatasets;
    class Options *options;
    class Pool *pool;
    //map<unsigned int, UserDataset> *userDatasets;

public slots:
    void displayDataset(unsigned int datasetIndex);

private:
    void updateDisplay(void);
    Ui::BrowseDatasetsClass ui;

private slots:
    void on_pushButton_2_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent *event);
    void sectionClicked ( int logicalIndex );
};

#endif // BROWSEDATASETS_H
