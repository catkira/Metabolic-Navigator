#include "metabolicpathviewer.h"
#include "Algorithm"
#include "Helper.h"

#include "Reaction.h"
#include "MetaboliteObject.h"


using namespace std;

MetabolicPathViewer::MetabolicPathViewer(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

MetabolicPathViewer::~MetabolicPathViewer()
{

}

void MetabolicPathViewer::updateAvailablePaths(void)
{
    // sort paths by path length
    sortBySecond<MetabolicPath<Reaction,MetaboliteObject>,unsigned int> functor;
    sort(pathways.begin(), pathways.end(),functor);

    pathwaysIterator = pathways.begin();
    ui.tableWidget->setRowCount(pathways.size());
    QTableWidgetItem *twi;
    char buf[10];
    QString qbuf;
    unsigned int n=0;
    while(pathwaysIterator != pathways.end())
    {
        qbuf = QString("path ") + QString().setNum(n+1);
        twi = new QTableWidgetItem(qbuf);
        ui.tableWidget->setItem(n,0,twi);

        _itoa_s(pathwaysIterator->second, &buf[0],10,10);
        twi = new QTableWidgetItem(buf);
        ui.tableWidget->setItem(n,1,twi);
        pathwaysIterator++;
        n++;
    }
}

void MetabolicPathViewer::on_tableWidget_itemSelectionChanged()
{
    unsigned int index=ui.tableWidget->currentRow();
    string buf;
    pathways[index].first.generateTextOutput(&buf,pool);
    ui.textEdit->setText(buf.c_str());
}