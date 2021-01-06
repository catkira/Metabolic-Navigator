#include "metabolitesviewer.h"
#include "MetabolicTree.h"
#include "TreeNode.h"
#include "Compound.h"
#include "MetaboliteObject.h"

MetabolitesViewer::MetabolitesViewer(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

MetabolitesViewer::~MetabolitesViewer()
{

}


void MetabolitesViewer::updateTable(void)
{
    ui.tableWidget->setRowCount(tree->treeNodesPool.size());
    
    T_treeNodesPoolIterator it;
    QTableWidgetItem *twi;
    unsigned int row = 0;
    ui.tableWidget->setSortingEnabled(false);
    it = tree->treeNodesPool.begin();
    while(it != tree->treeNodesPool.end())
    {
        twi = new QTableWidgetItem();
        twi->setData(Qt::UserRole, row);
        twi->setText(it->second.metabolite->compound->getName().c_str());
        ui.tableWidget->setItem(row,0,twi);

        twi = new QTableWidgetItem();
        twi->setText(it->second.metabolite->compound->cNumber.c_str());
        ui.tableWidget->setItem(row,1,twi);

        twi = new QTableWidgetItem();
        twi->setText(it->second.metabolite->labelText().c_str());
        ui.tableWidget->setItem(row,2,twi);

        ++row;
        ++it;
    }
    ui.tableWidget->setSortingEnabled(true);

}