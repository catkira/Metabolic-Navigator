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

#ifndef SVGWINDOW_H
#define SVGWINDOW_H

#include <QPoint>
#include <QScrollArea>
#include <QString>
#include <QObject>

class QKeyEvent;
class QMouseEvent;

class SvgWindow : public QScrollArea
{
Q_OBJECT
public:
    enum RendererType { Native, OpenGL, Image };

    SvgWindow();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void openFile(const QString &file);
    void setRenderer(RendererType type = Native);
    void setHighQualityAntialiasing(bool highQualityAntialiasing);

private:
    QPoint mousePressPos;
    QPoint scrollBarValuesOnMousePress;
    QString currentPath;
    RendererType renderer;

    bool highQualityAntialiasing;
private slots:
    void clickOnView(unsigned int, double, double, QPoint);

signals:
    void rightClick(double, double, QPoint);
    void doubleClick(double, double, QPoint);
};

#endif
