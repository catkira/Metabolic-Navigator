#include "browsecompounds.h"

using namespace std;

BrowseCompounds::BrowseCompounds(QWidget *parent, Options *options)
    : QDialog(parent)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"C-Number"<<"Name"<<"Formula"<<"#isomeres";
    ui.tableWidget->setHorizontalHeaderLabels(labels);
    BrowseCompounds::options = options;
    if(options!=0)
        options->loadWindowOptions(this, "BrowseCompounds");
}

BrowseCompounds::~BrowseCompounds()
{

}

void BrowseCompounds::showSelected(void)
{
    on_pushButton_3_clicked();
    show();
}


void BrowseCompounds::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "BrowseCompounds");
    event->accept();
}

void BrowseCompounds::updateDisplay(void)
{
    QTableWidgetItem *twi;
    ui.tableWidget->setRowCount(displayCompounds.size());
    unsigned int index;
    Compound *currentCompound;
    for(unsigned int n=0;n<displayCompounds.size();n++)
    {
        //index = displayCompounds[n].second;
        currentCompound = displayCompounds[n].second;

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].cNumber.c_str());
        twi->setText(currentCompound->cNumber.c_str());
        ui.tableWidget->setItem(n,0,twi);

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].names[0].c_str());
        twi->setText(currentCompound->names[0].c_str());
        ui.tableWidget->setItem(n,1,twi);

        twi=new QTableWidgetItem();
        //twi->setText((*pool->compounds)[index].formula.text().c_str());
        twi->setText(currentCompound->formula.text().c_str());
        ui.tableWidget->setItem(n,2,twi);

        twi=new QTableWidgetItem();
        //twi->setText(QString().setNum((*pool->compounds)[index].isomeres.size()));
        twi->setText(QString().setNum(currentCompound->isomeres.size()));
        ui.tableWidget->setItem(n,3,twi);
    }
    ui.tableWidget->resizeRowsToContents();
}

void BrowseCompounds::on_pushButton_clicked()
{
}

void BrowseCompounds::on_pushButton_2_clicked()
{
    displayCompounds.clear();
    map<string,Compound>::iterator compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        displayCompounds.push_back(pair<string,Compound*> (string(""),&(compoundsIterator->second)));
        compoundsIterator++;
    }
    updateDisplay();

}

void BrowseCompounds::on_pushButton_3_clicked()
{
    displayCompounds.clear();
    set<string>::iterator selectedCompoundsIterator = pool->selectedCompounds->begin();
    while(selectedCompoundsIterator != pool->selectedCompounds->end())
    {
        displayCompounds.push_back(pair<string,Compound*> (string(""),&(*pool->compounds)[*selectedCompoundsIterator]));
        selectedCompoundsIterator++;
    }
    updateDisplay();
}

void BrowseCompounds::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    ui.listWidget->clear();
    ui.listWidget_2->clear();
    map<Reaction *, unsigned int>::iterator linkedReactionsIterator;

    myAlgorithms->validateLinkedReactions();
    if(displayCompounds.size()>=index)
    {
        linkedReactionsIterator = displayCompounds[index].second->linkedReactions.begin();
        for(unsigned int n=0;n<displayCompounds[index].second->names.size();n++)
        {
            ui.listWidget->addItem(QString(displayCompounds[index].second->names[n].c_str()));
        }
        for(unsigned int n=0;n<displayCompounds[index].second->linkedReactions.size();n++)
        {
            ui.listWidget_2->addItem(QString(linkedReactionsIterator->first->rNumber.c_str()));
            linkedReactionsIterator++;
        }
    }
    else
    {
    }
}

void BrowseCompounds::on_tableWidget_cellDoubleClicked(int a,int b)
{
    unsigned int index=ui.tableWidget->currentRow();
    emit metaboliteSelected(QString(displayCompounds[index].second->cNumber.c_str()));
}

void BrowseCompounds::on_listWidget_2_itemDoubleClicked(QListWidgetItem* item)
{
    emit showReaction(item->text());
}