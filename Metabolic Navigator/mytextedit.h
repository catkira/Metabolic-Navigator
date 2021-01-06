#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>
#include "ui_mytextedit.h"

class MyTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    MyTextEdit(QWidget *parent = 0);
    ~MyTextEdit();

private:
    Ui::MyTextEditClass ui;
    void keyPressEvent(QKeyEvent *event);
signals:
    void returnPressed(void);

};

#endif // MYTEXTEDIT_H
