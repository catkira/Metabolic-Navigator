#include "mylabel.h"
#include <QPainter>
#include <QPen>


MyLabel::MyLabel(QString &text, QWidget *parent)
    : QLabel(parent)
{
    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, text);

    QImage image(size.width() + 12, size.height() + 12,
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
                          25, 25);

    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
    painter.end();

    setPixmap(QPixmap::fromImage(image));
    setFocusPolicy(Qt::ClickFocus);
    isSelected=false;

    labelText = text;
}

MyLabel::MyLabel(QWidget *parent)
    : QLabel(parent)
{
    //ui.setupUi(this);
    MyLabel::MyLabel(QString("blabla"),parent);
    //labelText="blabla";
}


MyLabel::~MyLabel()
{
    int a=0;
}

void MyLabel::setText(QString text)
{
        QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, text);

    QImage image(size.width() + 12, size.height() + 12,
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
                          25, 25);

    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
    painter.end();

    setPixmap(QPixmap::fromImage(image));
    setFocusPolicy(Qt::ClickFocus);
    isSelected=false;

    labelText = text;
}

void MyLabel::markSelected(void)
{
    if(isSelected==true)
        return;
    isSelected=true;
    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, labelText);

    QImage image(size.width() + 12, size.height() + 12,
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);

    QPainter painter;
    QPen pen;
    pen.setWidth(3);
    painter.begin(&image);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
                          25, 25);

    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, labelText);
    painter.end();

    setPixmap(QPixmap::fromImage(image));
}

void MyLabel::deselect(void)
{
    if(!isSelected)
        return;
    isSelected=false;

    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, labelText);

    QImage image(size.width() + 12, size.height() + 12,
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
                          25, 25);

    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, labelText);
    painter.end();

    setPixmap(QPixmap::fromImage(image));
}

void MyLabel::getSocketForPosition(QPoint, QPoint*, QPoint*)
{
}


void MyLabel::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
    {
        emit deleteMetabolite();
//        close();
    }
}

void MyLabel::mouseMoveEvent(QMouseEvent *event)
{
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - dragStartPosition).manhattanLength()
             < QApplication::startDragDistance())
            return;
        dragStartPosition = event->globalPos();
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << QPoint(event->pos() - rect().topLeft());
        void * pointer = this;
        dataStream.writeRawData((char*)&pointer,4);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/metabolic navigator", itemData);
        mimeData->setText(labelText);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - rect().topLeft());
        const QPixmap *pm = pixmap();
        drag->setPixmap(*pixmap());

        hide();

        if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
            //close();
            show();
        else
            show();

}


void MyLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        event->accept();
        emit rightClick(event->globalPos());
    }
    if(event->button() == Qt::LeftButton)
    {
        emit deselectAll();
        markSelected();
    }
}

