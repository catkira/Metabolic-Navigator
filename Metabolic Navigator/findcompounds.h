#ifndef FINDCOMPOUNDS_H
#define FINDCOMPOUNDS_H

#include <QDialog>
#include <QCloseEvent>
#include "ui_findcompounds.h"
//#include <map>
#include "Options.h"
#include "MyAlgorithms.h"

class FindCompounds : public QDialog
{
    Q_OBJECT

public:
    FindCompounds(QWidget *parent = 0, Options *options=0);
    ~FindCompounds();
    class Options *options;
    class MyAlgorithms *myAlgorithms;


private:
    Ui::FindCompoundsClass ui;
private slots:
    void on_pushButton_clicked();
    void on_radioButtonKeggID_toggled(bool);
    void on_radioButtonName_toggled(bool);
    void on_radioButtonFormula_toggled(bool);
    void closeEvent(QCloseEvent *event);
signals:
    void addLogLine(QString);
    void showSelectedCompounds(void);

};

#endif // FINDCOMPOUNDS_H
