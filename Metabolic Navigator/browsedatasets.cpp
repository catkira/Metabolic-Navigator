#include "browsedatasets.h"
#include <QHeaderView>
#include "UserDataset.h"
#include "Options.h"
#include "pool.h"


using namespace std;

BrowseDatasets::BrowseDatasets(QWidget *parent, Options *options)
    : QDialog(parent)
{
    ui.setupUi(this);
    QStringList labels;
    labels<<"Author"<<"R-Number/Name"<<"Confidence";
    ui.tableWidget->setHorizontalHeaderLabels(labels);
    BrowseDatasets::options = options;
    if(options!=0)
        options->loadWindowOptions(this, "BrowseDatasets");
    connect(ui.tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sectionClicked(int)));

    ui.tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Custom);
    ui.tableWidget->horizontalHeader()->setResizeMode(1,QHeaderView::Custom);
    ui.tableWidget->horizontalHeader()->setResizeMode(2,QHeaderView::Custom);
}

BrowseDatasets::~BrowseDatasets()
{

}

void BrowseDatasets::closeEvent(QCloseEvent *event)
{
    if(options!=0)
        options->saveWindowOptions(this, "BrowseDatasets");
    event->accept();
}

void BrowseDatasets::updateDisplay(void)
{
    ui.tableWidget->setRowCount(pool->userDatasets->size());
    QTableWidgetItem *twi;
    char buf[10];
    for(unsigned int n=0;n<pool->userDatasets->size();n++)
    {
        twi=new QTableWidgetItem();
        twi->setText((*pool->userDatasets)[n].scientist.c_str());
        ui.tableWidget->setItem(n,0,twi);
        twi=new QTableWidgetItem();
        twi->setText((*pool->userDatasets)[n].keggID.c_str());
        ui.tableWidget->setItem(n,1,twi);
        twi=new QTableWidgetItem();
//        twi->setText((*userDatasets)[n].equation.text().c_str());
        _itoa_s((*pool->userDatasets)[n].confidence,&buf[0],10,10);
        twi->setText(buf);
        ui.tableWidget->setItem(n,2,twi);
    }
    ui.tableWidget->resizeRowsToContents();
}

void BrowseDatasets::on_pushButton_clicked()
{
    displayDatasets.clear();
    for(unsigned int n=0;n<pool->userDatasets->size();n++)
        displayDatasets.push_back(pair<string,unsigned int> (string(""),n));
    updateDisplay();
}

void BrowseDatasets::displayDataset(unsigned int datasetIndex)
{
}


void BrowseDatasets::sectionClicked ( int logicalIndex )
{
}

void BrowseDatasets::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    if(displayDatasets.size()>=index)
    {
        ui.lineEdit_2->setText((*pool->userDatasets)[displayDatasets[index].second].equation->text(ui.checkBox_2->isChecked()).c_str());
        ui.textBrowser->setText((*pool->userDatasets)[displayDatasets[index].second].comment.c_str());
        if((*pool->userDatasets)[displayDatasets[index].second].accepted == true)
        {
            ui.lineEdit_5->setText("yes");
        }
        else
            ui.lineEdit_5->setText("no");
        if((*pool->userDatasets)[displayDatasets[index].second].excluded == true)
        {
            ui.lineEdit_6->setText("yes");
        }
        else
            ui.lineEdit_6->setText("no");
        if((*pool->userDatasets)[displayDatasets[index].second].equation->globalCompartments.size()!=0)
        {
            QString temp;
            set<string>::iterator globalCompartmentsIterator;
            globalCompartmentsIterator = (*pool->userDatasets)[displayDatasets[index].second].equation->globalCompartments.begin();
            while(globalCompartmentsIterator != (*pool->userDatasets)[displayDatasets[index].second].equation->globalCompartments.end())
            {
                temp += QString(globalCompartmentsIterator->c_str()) + " ";
                globalCompartmentsIterator++;
            }
            ui.lineEdit_3->setText(temp);
        }
        else
            ui.lineEdit_3->setText("see equation");
        ui.lineEdit->setText((*pool->userDatasets)[displayDatasets[index].second].equation->rawEquation.c_str());
    }
    else
    {
        ui.lineEdit->clear();
        ui.lineEdit_2->clear();
        ui.lineEdit_3->clear();
        ui.lineEdit_5->clear();
        ui.lineEdit_6->clear();
        ui.textBrowser->clear();
    }
}

void BrowseDatasets::on_pushButton_2_clicked()
{

}