#include "browsereactions.h"
#include <QHeaderView>
#include <Algorithm>

using namespace std;

BrowseReactions::BrowseReactions(QWidget *parent, Options *options)
    : QDialog(parent)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"R-Number"<<"Equation"<<"Source"<<"Compartments";
    ui.tableWidget->setHorizontalHeaderLabels(labels);
    BrowseReactions::options = options;
    if(options!=0)
        options->loadWindowOptions(this, "BrowseReactions");
    connect(ui.tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sectionClicked(int)));
}

BrowseReactions::~BrowseReactions()
{

}

void BrowseReactions::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "BrowseReactions");
    event->accept();
}


void BrowseReactions::showSelected(void)
{
    show();
    on_pushButton_2_clicked();
}

void BrowseReactions::updateDisplay(void)
{
    QTableWidgetItem *twi;
    QString temp;
    set<string>::iterator globalCompartmentsIterator;
    unsigned int i;
    unsigned int index;
    unsigned int displayReactionsSize=displayReactions.size();

    ui.tableWidget->setRowCount(displayReactions.size());
    Reaction *reaction;
    for(unsigned int n=0;n<displayReactionsSize;n++)
    {
        reaction = displayReactions[n].second;

        twi=new QTableWidgetItem(reaction->rNumber.c_str(),0);
        ui.tableWidget->setItem(n,0,twi);

        twi=new QTableWidgetItem(reaction->equation->text().c_str(),0);
        ui.tableWidget->setItem(n,1,twi);

        twi=new QTableWidgetItem(reaction->source.c_str(),0);
        if(reaction->dataset != -1)
        {
            if((*pool->userDatasets)[reaction->dataset].accepted==true)        
                twi->setBackground(Qt::green);
            else
                twi->setBackground(Qt::yellow);
        }
        ui.tableWidget->setItem(n,2,twi);

        if(reaction->dataset==-1)
        {
            twi=new QTableWidgetItem();
            twi->setText("");
            ui.tableWidget->setItem(n,3,twi);
            continue;
        }
        twi=new QTableWidgetItem();
        temp="";
        //if((*pool->reactions)[index].equation->compartments.size()!=0)
        if(reaction->equation->hasMetaboliteCompartments() == true)
        {
            temp="-> see compounds";
        }
        else
        {
            globalCompartmentsIterator = reaction->equation->globalCompartments.begin();
            for(unsigned int m=0;m<reaction->equation->globalCompartments.size();m++)
            {
                temp+=QString(globalCompartmentsIterator->c_str()) + " ";
                globalCompartmentsIterator++;
            }
        }
        twi->setText(temp);
        ui.tableWidget->setItem(n,3,twi);
    }
    ui.tableWidget->resizeRowsToContents();
}

void BrowseReactions::on_pushButton_clicked()
{
    displayReactions.clear();
    for(unsigned int n=0;n<pool->reactions->size();n++)
        displayReactions.push_back(pair<string,Reaction*> (string(""),&(*pool->reactions)[n]));
    updateDisplay();
}

void BrowseReactions::on_pushButton_2_clicked()
{
    displayReactions.clear();

    set<Reaction*>::iterator selectedReactionsIterator;
    selectedReactionsIterator = pool->selectedReactions2->begin();
    while(selectedReactionsIterator != pool->selectedReactions2->end())
    {
        displayReactions.push_back(pair<string,Reaction*> (string(""),
            *selectedReactionsIterator));
        ++selectedReactionsIterator;
    }

    updateDisplay();
}

void BrowseReactions::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    QString tempString;
    map<pair<string,string>,string>::iterator rpairIterator;
    if(displayReactions.size()>=index)
    {
        ui.lineEdit->setText(displayReactions[index].second->equation->text(ui.checkBox_2->isChecked(), true, pool).c_str());
        ui.lineEdit_2->setText(displayReactions[index].second->names[0].c_str());
        if(ui.checkBox->isChecked())
            emit displayDataset(displayReactions[index].second->dataset);
        rpairIterator = displayReactions[index].second->rpair.begin();
        ui.comboBox->clear();
        for(unsigned int n=0;n<displayReactions[index].second->rpair.size();n++)
        {
            tempString = string(rpairIterator->first.first + string(" ") +
                rpairIterator->first.second + string(" ") + rpairIterator->second ).c_str();
            ui.comboBox->insertItem(n,tempString);
            rpairIterator++;
        }
    }
    else
    {
        ui.lineEdit->clear();
        ui.lineEdit_2->clear();
        ui.comboBox->clear();
    }
}

void BrowseReactions::showReaction(QString rNumber)
{
    string rNumber_s = rNumber.toLatin1();
    for(unsigned int n=0;n<displayReactions.size();n++)
    {
        if(displayReactions[n].second->rNumber == rNumber_s)
        {
            // doesnt work because of QT bug
            ui.tableWidget->scrollToItem(ui.tableWidget->itemAt(n,0),QAbstractItemView::PositionAtTop);
            return;
        }
    }
}

void BrowseReactions::sectionClicked ( int logicalIndex )
{
    if(logicalIndex==3)
        return;
    vector<pair<string,Reaction*>> oldDisplayReactions = displayReactions;
    displayReactions.clear();
    for(unsigned int n=0;n<ui.tableWidget->rowCount();n++)
    {
        if(logicalIndex==0)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->rNumber,oldDisplayReactions[n].second));
        if(logicalIndex==1)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->equation->text(),oldDisplayReactions[n].second));
        if(logicalIndex==2)    
            displayReactions.push_back(pair<string,Reaction*> (oldDisplayReactions[n].second->source,oldDisplayReactions[n].second));
    }
    sortByFirst functor;
    std::sort(displayReactions.begin(), displayReactions.end(),functor);
    updateDisplay();
}
