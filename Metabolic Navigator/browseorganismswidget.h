#ifndef BROWSEORGANISMSWIDGET_H
#define BROWSEORGANISMSWIDGET_H

#include <QDockWidget>
#include "ui_browseorganismswidget.h"

class Organism;
class OrganismsBuildRule;
class BuildOrganismWidget;
class Pool;

class BrowseOrganismsWidget : public QDockWidget
{
    Q_OBJECT

public:
    BrowseOrganismsWidget(QWidget *parent = 0, Pool *pool = 0);
    ~BrowseOrganismsWidget();
    void updateTable(void);

public slots:
    void update(unsigned int signal);

private:
    Ui::BrowseOrganismsWidgetClass ui;
    Pool *pool;

    BuildOrganismWidget *buildWidget;

private slots:
    void on_pushButton_clicked();
};

#endif // BROWSEORGANISMSWIDGET_H
