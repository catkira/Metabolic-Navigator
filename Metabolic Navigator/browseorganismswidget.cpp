#include "browseorganismswidget.h"
#include "buildorganismwidget.h"
#include "pool.h"
#include "Organism.h"
#include "defines.h"

BrowseOrganismsWidget::BrowseOrganismsWidget(QWidget *parent, Pool *pool)
    : QDockWidget(parent), pool(pool)
{
    ui.setupUi(this);
    buildWidget = new BuildOrganismWidget(ui.groupBox);
    QStringList labels;
    labels<<"Name"<<"Reactions";
    ui.tableWidget->setHorizontalHeaderLabels(labels);

    ui.groupBox->setVisible(false);
}

BrowseOrganismsWidget::~BrowseOrganismsWidget()
{
    delete buildWidget;
}

void BrowseOrganismsWidget::updateTable(void)
{
    QTableWidgetItem *twi;

    ui.tableWidget->setRowCount(pool->organisms.size());

    unsigned int n=0;
    T_organismsIterator organismsIterator = pool->organisms.begin();
    while(organismsIterator != pool->organisms.end())
    {
        twi=new QTableWidgetItem(organismsIterator->second.name().c_str(),0);
        ui.tableWidget->setItem(n,0,twi);

        twi=new QTableWidgetItem(QString().setNum(organismsIterator->second.reactions.size()),0);
        ui.tableWidget->setItem(n,1,twi);

        ++n;
        ++organismsIterator;
    }
    ui.tableWidget->resizeColumnsToContents();
}

void BrowseOrganismsWidget::on_pushButton_clicked()
{
    updateTable();
}

void BrowseOrganismsWidget::update(unsigned int signal)
{
    if(signal == UPDATE_ORGANISMS)
        updateTable();
}