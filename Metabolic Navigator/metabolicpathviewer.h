#ifndef METABOLICPATHVIEWER_H
#define METABOLICPATHVIEWER_H

#include <QDialog>
#include "ui_metabolicpathviewer.h"
#include <map>
#include "MetabolicPath.h"

class Pool;
class Reaction;
class MetaboliteObject;

class MetabolicPathViewer : public QDialog
{
    Q_OBJECT

public:
    MetabolicPathViewer(QWidget *parent = 0);
    ~MetabolicPathViewer();

    std::vector<std::pair<MetabolicPath<Reaction,MetaboliteObject>,unsigned int>> pathways;
    std::vector<std::pair<MetabolicPath<Reaction,MetaboliteObject>,unsigned int>>::iterator pathwaysIterator;
    Pool *pool;
public slots:
    void updateAvailablePaths(void);

private:
    Ui::MetabolicPathViewerClass ui;

private slots:
    void on_tableWidget_itemSelectionChanged();
};

#endif // METABOLICPATHVIEWER_H
