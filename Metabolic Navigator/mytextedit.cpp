#include "mytextedit.h"

MyTextEdit::MyTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    ui.setupUi(this);
}

MyTextEdit::~MyTextEdit()
{

}

void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return && event->modifiers() == Qt::ShiftModifier)
        emit returnPressed();
    else
        QTextEdit::keyPressEvent(event); 
}