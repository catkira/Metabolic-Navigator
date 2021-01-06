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

#include "svgviewer.h"
#include "svgview.h"
#include <QSvgRenderer>
#include "spirit_include.h"

using namespace std;
using namespace boost::spirit;

SvgViewer::SvgViewer()
    : QMainWindow()
{
    area = new SvgWindow;

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));

    menuBar()->addMenu(fileMenu);

    QMenu *rendererMenu = new QMenu(tr("&Renderer"), this);
    nativeAction = rendererMenu->addAction(tr("&Native"));
    nativeAction->setCheckable(true);
    nativeAction->setChecked(true);
    #ifndef QT_NO_OPENGL
    glAction = rendererMenu->addAction(tr("&OpenGL"));
    glAction->setCheckable(true);
    #endif
    imageAction = rendererMenu->addAction(tr("&Image"));
    imageAction->setCheckable(true);

    #ifndef QT_NO_OPENGL
    rendererMenu->addSeparator();
    highQualityAntialiasingAction = rendererMenu->addAction(tr("&High Quality Antialiasing"));
    highQualityAntialiasingAction->setEnabled(false);
    highQualityAntialiasingAction->setCheckable(true);
    highQualityAntialiasingAction->setChecked(false);
    connect(highQualityAntialiasingAction, SIGNAL(toggled(bool)), this, SLOT(setHighQualityAntialiasing(bool)));
    #endif

    QActionGroup *rendererGroup = new QActionGroup(this);
    rendererGroup->addAction(nativeAction);
    #ifndef QT_NO_OPENGL
    rendererGroup->addAction(glAction);
    #endif
    rendererGroup->addAction(imageAction);

    menuBar()->addMenu(rendererMenu);

    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(rendererGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(setRenderer(QAction *)));

    setCentralWidget(area);
    title_ = "SVG Viewer";
    setWindowTitle(title_);

    connect(area, SIGNAL(rightClick(double, double, QPoint)), this, SLOT(rightClick(double, double, QPoint)));
    connect(area, SIGNAL(doubleClick(double, double, QPoint)), this, SLOT(doubleClick(double, double, QPoint)));
}

void SvgViewer::rightClick(double x, double y, QPoint pos)
{
    //p.x();
    std::vector<PlainFileEntry>::iterator plainFileEntriesIterator;
    plainFileEntriesIterator = plainFileEntries.begin();
    string matchingNode;
    while(plainFileEntriesIterator != plainFileEntries.end())
    {
        if(x > (plainFileEntriesIterator->x - plainFileEntriesIterator->width/2) && 
            x < (plainFileEntriesIterator->x + plainFileEntriesIterator->width/2) &&
            y > (plainFileEntriesIterator->y - plainFileEntriesIterator->height/2) &&
            y < (plainFileEntriesIterator->y + plainFileEntriesIterator->height/2))
        {
            matchingNode = plainFileEntriesIterator->nodeId;
            break;
        }
        ++plainFileEntriesIterator;
    }

    if (!matchingNode.empty())
    {
        emit nodeClicked(matchingNode.c_str(), pos);
        emit nodeRightClicked(matchingNode.c_str(), pos);
    }
}

void SvgViewer::doubleClick(double x, double y, QPoint pos)
{
    //p.x();
    std::vector<PlainFileEntry>::iterator plainFileEntriesIterator;
    plainFileEntriesIterator = plainFileEntries.begin();
    string matchingNode;
    while(plainFileEntriesIterator != plainFileEntries.end())
    {
        if(x > (plainFileEntriesIterator->x - plainFileEntriesIterator->width/2) && 
            x < (plainFileEntriesIterator->x + plainFileEntriesIterator->width/2) &&
            y > (plainFileEntriesIterator->y - plainFileEntriesIterator->height/2) &&
            y < (plainFileEntriesIterator->y + plainFileEntriesIterator->height/2))
        {
            matchingNode = plainFileEntriesIterator->nodeId;
            break;
        }
        ++plainFileEntriesIterator;
    }

    if (!matchingNode.empty())
    {
        emit nodeDoubleClicked(matchingNode.c_str(), pos);
    }
}

void SvgViewer::setTitle(const QString &title)
{
    title_ = title;
}

void SvgViewer::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
    area->setHighQualityAntialiasing(highQualityAntialiasing);
}


void SvgViewer::saveZoomAndPosition(void)
{
    QSize size = qobject_cast<SvgNativeView *>(area->widget())->doc->defaultSize();

    int x = area->horizontalScrollBar()->value();
    int y = area->verticalScrollBar()->value();

    zoomFaktor = (double)area->widget()->width() / (double)size.width();
}


void SvgViewer::restoreZoomAndPosition(void)
{
        if(position.height() != -1)
        {
            QSize size = qobject_cast<SvgNativeView *>(area->widget())->doc->defaultSize();
            size.setHeight(size.height() * zoomFaktor);
            size.setWidth(size.width() * zoomFaktor);
            area->widget()->resize(size);

            /*
            area->horizontalScrollBar()->setValue();
            area->verticalScrollBar()->setValue(y);
            area->horizontalScrollBar()->update();
            area->verticalScrollBar()->update();
            */
        }
}


void SvgViewer::openFile(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open SVG File"),
                                                currentPath, "*.svg");
    else
        fileName = path;

    if (!fileName.isEmpty()) 
    {
        area->openFile(fileName);
        if (!fileName.startsWith(":/")) 
        {
            currentPath = fileName;
            //setWindowTitle(tr("%1 - SVGViewer").arg(currentPath));
            setWindowTitle(title_ + " - " + currentPath);
        }
    }
    plainFileEntries.clear();
}

void SvgViewer::setRenderer(QAction *action)
{
    #ifndef QT_NO_OPENGL
    highQualityAntialiasingAction->setEnabled(false);
    #endif

    if (action == nativeAction)
        area->setRenderer(SvgWindow::Native);
    #ifndef QT_NO_OPENGL
    else if (action == glAction) {
        area->setRenderer(SvgWindow::OpenGL);
        highQualityAntialiasingAction->setEnabled(true);
    }
    #endif
    else if (action == imageAction)
        area->setRenderer(SvgWindow::Image);
}


void SvgViewer::readPlainFile(const QString &filename)
{
    plainFileEntries.clear();

    file_iterator<char> first,last;
    first = file_iterator<char>(string(filename.toAscii()));
    if(!first)
    {
        cout << "Error opening file!";
        return;
    }
    last = first.make_end();

    typedef scanner<file_iterator<char>> scanner_t;
    typedef rule<scanner_t> rule_t;

    PlainFileEntry temp;

    rule_t nodeId = *(anychar_p - space_p - eol_p);
    rule_t nodeLine = str_p("node")
        >> space_p
        >> nodeId[assign_a(temp.nodeId)] >> *space_p
        >> real_p[assign_a(temp.x)] >> space_p
        >> real_p[assign_a(temp.y)] >> space_p
        >> real_p[assign_a(temp.width)] >> space_p
        >> real_p[assign_a(temp.height)] >> space_p
        >> *(anychar_p - eol_p ) >> eol_p;
    rule_t anyLine = *(anychar_p - eol_p) >> eol_p;

    rule_t main = *(nodeLine[push_back_a(plainFileEntries, temp)] | anyLine);

    BOOST_SPIRIT_DEBUG_NODE(nodeId);
    BOOST_SPIRIT_DEBUG_NODE(nodeId);
    BOOST_SPIRIT_DEBUG_NODE(nodeLine);
    BOOST_SPIRIT_DEBUG_NODE(anyLine);

    if(!parse(first, last, main).full)
        throw std::exception("could not parse plain file");

}