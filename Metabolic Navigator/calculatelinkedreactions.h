#ifndef CALCULATELINKEDREACTIONS_H
#define CALCULATELINKEDREACTIONS_H

#include <QDialog>
#include "ui_calculatelinkedreactions.h"
#include "MyAlgorithms.h"

class CalculateLinkedReactions : public QDialog
{
    Q_OBJECT

public:
    CalculateLinkedReactions(QWidget *parent = 0);
    ~CalculateLinkedReactions();
    MyAlgorithms *myAlgorithms;

private:
    Ui::CalculateLinkedReactionsClass ui;

private slots:
    void on_pushButton_clicked();
};

#endif // CALCULATELINKEDREACTIONS_H
