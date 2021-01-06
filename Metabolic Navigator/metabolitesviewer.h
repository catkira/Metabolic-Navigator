#ifndef METABOLITESVIEWER_H
#define METABOLITESVIEWER_H

#include <QDialog>
#include "ui_metabolitesviewer.h"

class MetabolicTree;

class MetabolitesViewer : public QDialog
{
    Q_OBJECT

public:
    MetabolitesViewer(QWidget *parent = 0);
    ~MetabolitesViewer();

    MetabolicTree *tree;
    void updateTable(void);

private:
    Ui::MetabolitesViewerClass ui;
};

#endif // METABOLITESVIEWER_H
