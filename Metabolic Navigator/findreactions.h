#ifndef FINDREACTIONS_H
#define FINDREACTIONS_H

#include <QDialog>
#include "ui_findreactions.h"
#include <map>
#include <vector>
#include "Reaction.h"
#include "Options.h"
#include <QCloseEvent>
#include "MyAlgorithms.h"


class FindReactions : public QDialog
{
    Q_OBJECT

public:
    FindReactions(QWidget *parent = 0, Options *options=0);
    ~FindReactions();

    class Options *options;
    class MyAlgorithms *myAlgorithms;

private:
    Ui::FindReactionsClass ui;

private slots:
    void on_checkBox_stateChanged(int);
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent *event);

signals:
    void addLogLine(QString);
    void showSelectedReactions(void);
};

#endif // FINDREACTIONS_H
