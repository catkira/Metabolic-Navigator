#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include "ui_mylabel.h"
#include "MetaboliteObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

class QDragEnterEvent;
class QDragMoveEvent;
class QFrame;

class MetaboliteObject;

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    MyLabel(QString &text, QWidget *parent = 0);
    MyLabel(QWidget *parent = 0);
    MetaboliteObject *metaboliteObject;
    void getSocketForPosition(QPoint, QPoint*, QPoint*);
    void markSelected(void);
    ~MyLabel();
public slots:
    void deselect(void);
    void setText(QString text);

private:
    Ui::MyLabelClass ui;
    QString labelText;
    QPoint dragStartPosition;
    bool isMoving;
    bool isSelected;
protected:
    void keyReleaseEvent(QKeyEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void rightClick(QPoint pos);
    void leftClick(void);
    void deselectAll(void);
    void deleteMetabolite();
};

#endif // MYLABEL_H
