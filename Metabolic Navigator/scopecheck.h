#ifndef SCOPECHECK_H
#define SCOPECHECK_H

#include <QDialog>
#include "ui_scopecheck.h"

class ScopeCheck : public QDialog
{
    Q_OBJECT

public:
    ScopeCheck(QWidget *parent = 0);
    ~ScopeCheck();

private:
    Ui::ScopeCheckClass ui;

private slots:
    void on_pushButton_2_clicked();
    void on_textBrowser_2_textChanged();
};

#endif // SCOPECHECK_H
