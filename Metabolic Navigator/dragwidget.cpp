#include "dragwidget.h"
#include <QPainter>

DragWidget::DragWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::white);
    setPalette(newPalette);
    setAcceptDrops(true);
}

DragWidget::~DragWidget()
{

}

void DragWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::black);
    QPoint P1,P2;
    connectionObjectsIterator = connectionObjects->begin();
    for(unsigned int n=0;n<connectionObjects->size();n++)
    {
        P1.setX(connectionObjectsIterator->second->leftMetabolite->imageObject->x() + 
            connectionObjectsIterator->second->leftMetabolite->imageObject->width()/2);
        P1.setY(connectionObjectsIterator->second->leftMetabolite->imageObject->y() + 
            connectionObjectsIterator->second->leftMetabolite->imageObject->height()/2);

        P2.setX(connectionObjectsIterator->second->rightMetabolite->imageObject->x() + 
            connectionObjectsIterator->second->rightMetabolite->imageObject->width()/2);
        P2.setY(connectionObjectsIterator->second->rightMetabolite->imageObject->y() + 
            connectionObjectsIterator->second->rightMetabolite->imageObject->height()/2);
        painter.drawLine(P1,P2);
        connectionObjectsIterator++;
    }
    //painter.drawRoundRect(QRectF(0.5, 0.5, 10, 10), 25, 25);

    painter.end();
}


void DragWidget::updateConnections(void)
{
    repaint();
}


void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void DragWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() & Qt::RightButton)
    {
        emit rightClick(event->globalPos());
    }
    if(event->button() & Qt::LeftButton)
    {
        emit leftClick(event->globalPos());
    }
    QWidget::mouseReleaseEvent(event);
}


void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/metabolic navigator")) 
    {
        if (children().contains(event->source())) 
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } 
        else 
        {
            event->acceptProposedAction();
        }
    } 
    else 
    {
        event->ignore();
    }
}

void DragWidget::dropEvent(QDropEvent *event)
{

    if (event->mimeData()->hasFormat("application/metabolic navigator")) 
    {
        const QMimeData *mime = event->mimeData();
        QByteArray itemData = mime->data("application/metabolic navigator");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString text;
        QPoint offset;
        MyLabel *senderImage;
        dataStream >> offset;
        dataStream.readRawData((char*)&senderImage,4);

        senderImage->move(event->pos() - offset);
        if (children().contains(event->source())) 
        {
            event->setDropAction(Qt::MoveAction);
            repaint();
            event->accept();
        } 
        else 
        {
            event->acceptProposedAction();
        }
    } 
    else {
        event->ignore();
    }
}

void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
}