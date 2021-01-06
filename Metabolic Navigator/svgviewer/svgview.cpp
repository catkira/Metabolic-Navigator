/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "svgview.h"

#include <QSvgRenderer>

#include <QApplication>
#include <QPainter>
#include <QImage>
#include <QWheelEvent>
#include <QtDebug>

SvgNativeView::SvgNativeView(const QString &file, QWidget *parent)
    : QWidget(parent)
{
    doc = new QSvgRenderer(file, this);
    connect(doc, SIGNAL(repaintNeeded()),
            this, SLOT(update()));
}

void SvgNativeView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setViewport(0, 0, width(), height());
    doc->render(&p);
}

QSize SvgNativeView::sizeHint() const
{
    if (doc)
        return doc->defaultSize();
    return QWidget::sizeHint();
}

void SvgNativeView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        event->ignore();

    if(event->button() == Qt::RightButton)
    {
        double x,y;
        QPainter painter(this);
        QSize origSize = doc->defaultSize();
        double scaleFactorX = (double)origSize.width()/(double)width();
        double scaleFactorY = (double)origSize.height()/(double)height();
        QPoint p = event->pos();

        x = (double)p.x() * (double)scaleFactorX / (double)painter.device()->physicalDpiX() * 1.332;
        y = (double)(height() - p.y()) * (double)scaleFactorY / (double)painter.device()->physicalDpiY() * 1.332;
        emit click(CLICK_TYPE_RIGHT, x,y, mapToParent(event->pos()));
    }
}

void SvgNativeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        event->ignore();

    if(event->button() == Qt::LeftButton)
    {
        double x,y;
        QPainter painter(this);
        QSize origSize = doc->defaultSize();
        double scaleFactorX = (double)origSize.width()/(double)width();
        double scaleFactorY = (double)origSize.height()/(double)height();
        QPoint p = event->pos();
        x = (double)p.x() * (double)scaleFactorX / (double)painter.device()->physicalDpiX() * 1.332;
        y = (double)(height() - p.y()) * (double)scaleFactorY / (double)painter.device()->physicalDpiY() * 1.332;
        emit click(CLICK_TYPE_DOUBLE, x,y, mapToParent(event->pos()));
    }
}

void SvgNativeView::wheelEvent(QWheelEvent *e)
{
    const double diff = 0.1;
    QSize size = doc->defaultSize();
    int width  = size.width();
    int height = size.height();
    if (e->delta() > 0) {
        width = int(this->width()+this->width()*diff);
        height = int(this->height()+this->height()*diff);
    } else {
        width  = int(this->width()-this->width()*diff);
        height = int(this->height()-this->height()*diff);
    }
    resize(width, height);
}

