#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QDragMoveEvent>
#include <QPainter>
#include "ui_dragwidget.h"
#include "MyLabel.h"
#include <map>
#include "ConnectionObject.h"
#include "MetaboliteObject.h"
#include "boost/shared_ptr.hpp"

class QDragEnterEvent;
class QDropEvent;
class QDragMoveEvent;

class DragWidget : public QWidget
{
    Q_OBJECT

public:
    DragWidget(QWidget *parent = 0);
    ~DragWidget();
    std::map<unsigned int, boost::shared_ptr<ConnectionObject>> *connectionObjects;
    std::map<unsigned int, boost::shared_ptr<ConnectionObject>>::iterator connectionObjectsIterator;
    std::map<std::string, MetaboliteObject*> *metabolitesPool;

public slots:
    void updateConnections(void);


private:
    Ui::DragWidgetClass ui;
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void paintEvent ( QPaintEvent * event );
signals:
    void rightClick(QPoint pos);
    void leftClick(QPoint pos);
};

#endif // DRAGWIDGET_H
