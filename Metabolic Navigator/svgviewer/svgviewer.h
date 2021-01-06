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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPoint>
#include <vector>
#include <string>

#include "svgwindow.h"

class QAction;

class PlainFileEntry
{
public:
    std::string nodeId;
    double x,y,width,height;
};

class SvgViewer : public QMainWindow
{
    Q_OBJECT

public:
    SvgViewer();
    SvgWindow *area;

public slots:
    void openFile(const QString &path = QString());
    void setRenderer(QAction *action);
    void setHighQualityAntialiasing(bool hq);
    void setTitle(const QString &title = QString());
    void readPlainFile(const QString &filename);
    void saveZoomAndPosition(void);
    void restoreZoomAndPosition(void);


private:
    QAction *nativeAction;
    QAction *glAction;
    QAction *imageAction;
    QAction *highQualityAntialiasingAction;
    QString currentPath;

    QString title_;

    double zoomFaktor;
    QRect position;

    std::vector<PlainFileEntry> plainFileEntries;
private slots:
    void rightClick(double, double, QPoint);
    void doubleClick(double, double, QPoint);
signals:
    void nodeClicked(const QString &nodeId, QPoint pos);
    void nodeRightClicked(const QString &nodeId, QPoint pos);
    void nodeDoubleClicked(const QString &nodeId, QPoint pos);

};

#endif
