#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "ui_searchdialog.h"

class Options;
class Pool;

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    SearchDialog(QWidget *parent = 0, Pool *pool = 0, Options *options=0);
    ~SearchDialog();
    class MyAlgorithms *myAlgorithms;
    class Options *options;
    Pool *pool;

public slots:
    void update(unsigned int);

private:
    Ui::SearchDialogClass ui;

private slots:
    void on_radioButtonNameID_toggled(bool);
    void on_lineEditFormula_textChanged(const QString &);
    void on_lineEditFormula_returnPressed();
    void on_lineEditNameID_returnPressed();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void closeEvent(QCloseEvent *event);

signals:
    void showSelectedCompounds(void);
    void showSelectedReactions(void);
    void addLogLine(QString);
};

#endif // SEARCHDIALOG_H
