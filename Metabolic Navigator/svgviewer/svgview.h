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

#ifndef SVGVIEW_H
#define SVGVIEW_H

#include <QWidget>
#include <QImage>
#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

class QPaintEvent;
class QSvgRenderer;
class QWheelEvent;

const int CLICK_TYPE_RIGHT = 1;
const int CLICK_TYPE_DOUBLE = 2;

class SvgNativeView : public QWidget
{
    Q_OBJECT

public:
    SvgNativeView(const QString &file, QWidget *parent=0);
    QPoint clickPos;
    QSvgRenderer *doc;

    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:

signals:
    void click(unsigned int type, double, double, QPoint);

};

#endif
