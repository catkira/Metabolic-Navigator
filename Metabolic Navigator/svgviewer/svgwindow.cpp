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

#include <QtGui>

#include "svgview.h"
#include "svgwindow.h"

SvgWindow::SvgWindow()
    : QScrollArea(),
      highQualityAntialiasing(false)
{
    QWidget *view = new QWidget(this);
    renderer = SvgWindow::Native;
    setWidget(view);
}

void SvgWindow::openFile(const QString &file)
{
    currentPath = file;
    setRenderer(renderer);
}

void SvgWindow::setRenderer(RendererType type)
{
    renderer = type;
    QWidget *view, *oldView;

    /*
    oldView = widget();
    disconnect(oldView);
    */

    if (renderer == OpenGL) 
    {
    } 
    else if (renderer == Image) 
    {
    } 
    else {
        view = new SvgNativeView(currentPath, this);
    }

    oldView = takeWidget(); // delete old view
    
    /*if(oldView != 0)
    {
        oldView->close();
        delete oldView;
    }*/

    connect(view, SIGNAL(click(unsigned int, double, double, QPoint)), this, SLOT(clickOnView(unsigned int, double, double, QPoint)));

    setWidget(view);
    view->show();

}

void SvgWindow::clickOnView(unsigned int type, double x, double y, QPoint pos)
{
    pos = mapToParent(pos);
    if(type ==  CLICK_TYPE_RIGHT)
        emit rightClick(x,y, pos);
    else if(type == CLICK_TYPE_DOUBLE)
        emit doubleClick(x,y, pos);
}


void SvgWindow::setHighQualityAntialiasing(bool hq)
{
    highQualityAntialiasing = hq;

    #ifndef QT_NO_OPENGL
    QWidget *view = widget();
    //if (renderer == OpenGL)
        //qobject_cast<SvgGLView *>(view)->setHighQualityAntialiasing(highQualityAntialiasing);

    #endif
}

void SvgWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mousePressPos = event->pos();
        scrollBarValuesOnMousePress.rx() = horizontalScrollBar()->value();
        scrollBarValuesOnMousePress.ry() = verticalScrollBar()->value();
        event->accept();
    }
    if(event->button() == Qt::RightButton)
    {

    }
}

void SvgWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressPos.isNull()) {
        event->ignore();
        return;
    }

    horizontalScrollBar()->setValue(scrollBarValuesOnMousePress.x() - event->pos().x() + mousePressPos.x());
    verticalScrollBar()->setValue(scrollBarValuesOnMousePress.y() - event->pos().y() + mousePressPos.y());
    horizontalScrollBar()->update();
    verticalScrollBar()->update();
    event->accept();
}

void SvgWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressPos = QPoint();
    event->accept();
}

void SvgWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
}
