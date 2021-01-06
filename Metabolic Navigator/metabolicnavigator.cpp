#include "metabolicnavigator.h"
#include <QSettings>
#include "browsecompoundswidget.h"
#include "browsereactionswidget.h"
#include "browseorganismswidget.h"
#include "Pool.h"
#include "MyAlgorithms.h"
#include "LogWidget.h"
#include "CommonsWidget.h"
#include "ImportThread.h"
#include <QFileDialog>
#include "Polynom.h"
#include <string>
#include "ImportFateMapsMu.h"
#include "LabelPositionAssistent.h"
#include <QMessageBox>
#include "palssonKeggConverter.h"

using namespace std;

MetabolicNavigator::MetabolicNavigator(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    options = new Options();
    options->settings = new QSettings("Charite Berlin", "Metabolic Navigator");
   // loadOptions();
    
    ui.setupUi(this);
    move(QPoint(500,200));

    pool = new Pool();
    myAlgorithms = new MyAlgorithms();
    logWidget = new LogWidget(this);
    importUserDatasets = new ImportUserDatasets(this);
    importKegg = new ImportKegg(this);
    browseDatasets = new BrowseDatasets(this, options);
    //findReactions = new FindReactions(this, options);
    //findCompounds = new FindCompounds(this, options);
    startupOptions = new StartupOptions(this, options);
    scopeCheck = new ScopeCheck(this);
    calculateLinkedReactions = new CalculateLinkedReactions(this);
    //metabolicWalker = new MetabolicWalker(this);
    pathFinder = new PathFinder(this, options);
    metabolicPathViewer = new MetabolicPathViewer(this);
    about = new About(this);
    svgViewer = new SvgViewer();
    searchDialog = new SearchDialog(this, pool, options);
    //browseDialog = new BrowseDialog(this, options, pool, myAlgorithms);
    browseCompoundsWidget = new BrowseCompoundsWidget(this, pool, myAlgorithms);
    browseReactionsWidget = new BrowseReactionsWidget(this, pool, myAlgorithms);
    browseOrganismsWidget = new BrowseOrganismsWidget(this, pool);
    commonsWidget = new CommonsWidget(this);
    labelPositionAssistent = new LabelPositionAssistent(0, pool, myAlgorithms, options);
    palssonKeggConverter = new PalssonKeggConverter(0, pool, myAlgorithms);
    labelPositionAssistent->svgViewer = svgViewer;

    if(options->settings->value("mainWindowState").toByteArray().size()==0)
    {
        addDockWidget(Qt::LeftDockWidgetArea,browseCompoundsWidget);
        addDockWidget(Qt::LeftDockWidgetArea,browseReactionsWidget);
        tabifyDockWidget(browseCompoundsWidget, browseReactionsWidget);
        tabifyDockWidget(browseReactionsWidget, browseOrganismsWidget);
        addDockWidget(Qt::BottomDockWidgetArea,logWidget);
        addDockWidget(Qt::TopDockWidgetArea,commonsWidget);
    }
    else
        restoreState(options->settings->value("mainWindowState").toByteArray());
 

    svgViewer->setTitle("Graph viewer");

    myAlgorithms->pool = pool;
    browseDatasets->pool = pool;
    importUserDatasets->pool = pool;
    importUserDatasets->myAlgorithms = myAlgorithms;
    importKegg->pool = pool;
    importKegg->myAlgorithms = myAlgorithms;
    calculateLinkedReactions->myAlgorithms = myAlgorithms;
    //metabolicWalker->pool = pool;
    //metabolicWalker->myAlgorithms = myAlgorithms;
    metabolicPathViewer->pool = pool;
    pathFinder->pool = pool;
    pathFinder->myAlgorithms = myAlgorithms;
    pathFinder->metabolicPathViewer = metabolicPathViewer;
    searchDialog->myAlgorithms = myAlgorithms;

    connect(myAlgorithms,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    connect(this,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    connect(importKegg,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    connect(importUserDatasets,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    //connect(metabolicWalker,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    connect(pathFinder,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    connect(searchDialog,SIGNAL(addLogLine(QString)),logWidget,SLOT(addLogLine(QString)));
    
    connect(importUserDatasets,SIGNAL(newDatasetsAvailable()),this,SLOT(newDatasetsAvailable()));

    connect(searchDialog,SIGNAL(showSelectedCompounds()),browseCompoundsWidget,SLOT(showSelectedCompounds()));
    connect(searchDialog,SIGNAL(showSelectedCompounds()),this,SLOT(showSelectedCompoundsSlot()));
    connect(searchDialog,SIGNAL(showSelectedReactions()),browseReactionsWidget,SLOT(showSelectedReactions()));
    connect(searchDialog,SIGNAL(showSelectedReactions()),this,SLOT(showSelectedReactionsSlot()));


    //connect(browseCompoundsWidget,SIGNAL(compoundSelected(QString)),metabolicWalker,SLOT(metaboliteSelected(QString)));

    connect(importUserDatasets,SIGNAL(updateCompartmentList()),pathFinder,SLOT(updateCompartmentList()));
    connect(importKegg,SIGNAL(updateCompartmentList()),pathFinder,SLOT(updateCompartmentList()));
    connect(myAlgorithms,SIGNAL(updateCompartmentList()),pathFinder,SLOT(updateCompartmentList()));

    connect(commonsWidget, SIGNAL(commonAction(QString)), this, SLOT(commonAction(QString)));


    // update signals
    connect(importUserDatasets,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));
    connect(importKegg,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));

    connect(this,SIGNAL(update(unsigned int)), browseOrganismsWidget, SLOT(update(unsigned int)));
    connect(this,SIGNAL(update(unsigned int)), pathFinder, SLOT(update(unsigned int)));
    connect(this,SIGNAL(update(unsigned int)), searchDialog, SLOT(update(unsigned int)));
    connect(this,SIGNAL(update(unsigned int)), browseCompoundsWidget, SLOT(update(unsigned int)));
    connect(this,SIGNAL(update(unsigned int)), browseReactionsWidget, SLOT(update(unsigned int)));
    connect(this,SIGNAL(update(unsigned int)), labelPositionAssistent, SLOT(update(unsigned int)));

    connect(importKegg,SIGNAL(importFinished()), this, SLOT(keggImportFinished()));

    options->loadWindowOptions(this, "MainWindow");

    pool->availableCompartments->insert("none");
    pathFinder->updateCompartmentList();

    //setCentralWidget(mainWidget);


    Polynome p1;
    Polynome p2;
    Polynome p3;

    p1 = "1*abc";
    p2 = "2*zzz";
    p3 = (p1*p2 + p1)*"3";
    p3 *= "ii";
    p3 *= "20";
    string text = p3.text();

    importMuThread = new ImportThread(this, ITF_MU);
    connect(importMuThread,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));

    importEnzymesThread = new ImportThread(this, ITF_ENZYMES);
    connect(importEnzymesThread,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));

    importOrganismThread = new ImportThread(this, ITF_ORGANISM);
    connect(importOrganismThread,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));

    setupDirs();
}

MetabolicNavigator::~MetabolicNavigator()
{
    importMuThread->terminate();
    delete importMuThread;
    importOrganismThread->terminate();
    delete importOrganismThread;

    delete labelPositionAssistent;
    delete palssonKeggConverter;

    delete about;
    delete metabolicPathViewer;
    delete logWidget;
    delete importUserDatasets;
    delete browseDatasets;
    delete importKegg;
    //delete metabolicWalker;
    delete pathFinder;
    delete svgViewer;
    delete searchDialog;
    delete browseCompoundsWidget;
    delete browseReactionsWidget;
    delete browseOrganismsWidget;
    delete commonsWidget;
    delete pool;
}

void MetabolicNavigator::keggImportFinished(void)
{
    QString title, message;
    title = "";
    message = "Do you want to import the carbon transition maps from mu as well?\nIf not, you can not trace atoms!";
    if(QMessageBox::question(0, title,message,QMessageBox::Yes | QMessageBox::No,QMessageBox::No)
        == QMessageBox::No)
        return;

    on_actionCarbon_Fate_Maps_triggered();

}


void MetabolicNavigator::showSelectedCompoundsSlot(void)
{
    if(dockWidgetArea(browseCompoundsWidget) == Qt::LeftDockWidgetArea)
    {
        browseCompoundsWidget->raise();
    }
}

void MetabolicNavigator::showSelectedReactionsSlot(void)
{
    if(dockWidgetArea(browseReactionsWidget) == Qt::LeftDockWidgetArea)
    {
        browseReactionsWidget->raise();
    }
}

void MetabolicNavigator::setupDirs(void)
{
    pool->applicationDir = string(QApplication::applicationDirPath().toAscii()) + "/";

    if(QFile::exists(pool->applicationDir.c_str() + QString("Data/enzyme")) == true)
    {
        pool->dataDir = pool->applicationDir.c_str() + string(QString("Data/").toAscii());
    }
    else if(QFile::exists("C:/Data/enzyme") == true)
    {
        pool->dataDir = "C:/Data/";
    }
    else
    {
        QDir dir;
        dir.setCurrent(pool->applicationDir.c_str());
        if(dir.exists("Data") == false)
        {
            dir.mkdir("Data");
        }
        pool->dataDir = pool->applicationDir.c_str() + string(QString("Data/").toAscii());

        // need to copy data files!
    }
}

void MetabolicNavigator::closeEvent(QCloseEvent *event)
{

    options->settings->setValue("mainWindowState", saveState());
    options->saveWindowOptions(this, "MainWindow");

    browseCompoundsWidget->closeEvent(&QCloseEvent());

    browseDatasets->close();
    logWidget->close();
    importUserDatasets->close();
    about->close();
    //metabolicWalker->close();
    pathFinder->close();
    svgViewer->close();
    searchDialog->close();
    labelPositionAssistent->close();
    palssonKeggConverter->close();

    event->accept();
}

void MetabolicNavigator::on_action_Quit_triggered()
{
    //saveOptionsToRegistry();
    //QApplication::exit(0);
    close();
}

void MetabolicNavigator::on_actionCharit_user_files_triggered()
{
    importUserDatasets->checkForDatasetFiles();
    importUserDatasets->show();
}

void MetabolicNavigator::on_actionDatasets_triggered()
{
    browseDatasets->show();
}

void MetabolicNavigator::on_actionKegg_triggered()
{
    //importKegg->checkForKeggFiles();
    //importKegg->show();


    pool->compounds->clear();
    pool->reactions->clear();
    pool->metabolitesPool->clear();

    importKegg->importCompoundAndReaction();

//    importKegg->importReaction();

}

void MetabolicNavigator::on_action_Startup_triggered()
{
    startupOptions->show();
}

void MetabolicNavigator::on_action_Log_window_triggered()
{
    logWidget->show();
}

void MetabolicNavigator::on_action_User_datasets_triggered()
{

}

void MetabolicNavigator::on_action_Scope_analysis_triggered()
{
    scopeCheck->show();
}

void MetabolicNavigator::on_actionFind_non_standard_metabolit_names_triggered()
{
    myAlgorithms->findReplaceableMetabolites();
}

void MetabolicNavigator::on_actionCalculate_linked_reactions_for_compounds_triggered()
{
    calculateLinkedReactions->show();
}

void MetabolicNavigator::on_action_Show_triggered()
{
}

void MetabolicNavigator::on_action_Path_finder_triggered()
{
    pathFinder->show();
}

void MetabolicNavigator::on_actionFix_compartments_triggered()
{
    myAlgorithms->fixCompartments();
}

void MetabolicNavigator::on_actionCalculate_missing_rpairs_triggered()
{
    myAlgorithms->calculateMissingRpairs();
}

void MetabolicNavigator::on_action_About_triggered()
{
    about->show();
}

void MetabolicNavigator::on_actionMetabolic_walker_triggered()
{
    //metabolicWalker->show();
}

void MetabolicNavigator::on_action_Graph_viewer_triggered()
{
    svgViewer->show();
}

void MetabolicNavigator::on_action_Search_window_triggered()
{
    searchDialog->show();
}

void MetabolicNavigator::on_actionCompounds_Widget_triggered()
{
    browseCompoundsWidget->show();
    //tabifyDockWidget(browseReactionsWidget, browseCompoundsWidget);
 
//    addDockWidget(Qt::TopDockWidgetArea,browseCompoundsWidget);
    //addDockWidget(Qt::TopDockWidgetArea,browseReactionsWidget);
 //   browseCompoundsWidget->show();
}

void MetabolicNavigator::on_actionReactions_2_triggered()
{
    browseReactionsWidget->show();
    //tabifyDockWidget(browseCompoundsWidget, browseReactionsWidget);
}

void MetabolicNavigator::on_actionCommon_actions_triggered()
{
    addDockWidget(Qt::BottomDockWidgetArea,commonsWidget);
}

void MetabolicNavigator::commonAction(QString action)
{
    if(action=="keggCompounds")
        importKegg->importCompound();
    else if(action=="keggReactions")
        importKegg->importReaction();
    else if(action=="importUser")
        importUserDatasets->show();
    else if(action=="importKegg")
        importKegg->show();
    else if(action=="createNetwork")
        pathFinder->show();
    //else if(action=="navigator")
      //  metabolicWalker->show();
}



void MetabolicNavigator::on_action_Organisms_triggered()
{
    browseOrganismsWidget->show();
    //tabifyDockWidget(browseReactionsWidget, browseOrganismsWidget);
}

void MetabolicNavigator::requestUpdateSlot(unsigned int signal)
{
    emit update(signal);
}

void MetabolicNavigator::on_actionSBML_2_triggered()
{
    QString fileName = QFileDialog::getOpenFileName();
    if(fileName != "")
    {
        ImportThread *importSBMLThread = new ImportThread(this, ITF_SBML);    
        importSBMLThread->pool = pool;
        importSBMLThread->myAlgorithms = myAlgorithms;
        importSBMLThread->bytes = fileName.toAscii();
        connect(importSBMLThread, SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));
        connect(importSBMLThread, SIGNAL(addLogLine(QString)), logWidget, SLOT(addLogLine(QString)));
        importSBMLThread->run();
    }
}

void MetabolicNavigator::on_actionCarbon_Fate_Maps_triggered()
{    
    importMuThread->pool = pool;
    importMuThread->myAlgorithms = myAlgorithms;

    unsigned int maxCount = 0,maxCount2=0;
    QFile file;
    QDir::setCurrent("D:/fatemap/");

    file.setFileName("compounds.txt");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open compounds");
        return;
    }
    importMuThread->compoundBytes = file.readAll().constData();
    file.close();

    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importMuThread->compoundBytes.find("AuxInfo",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }

    // reactions
    file.setFileName("reactions.txt");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open reactions");
        return;
    }
    importMuThread->reactionBytes = file.readAll().constData();
    file.close();

    pos = 0;
    while(pos != string::npos)
    {
        pos = importMuThread->reactionBytes.find("<->",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount2;
    }

    //importMuThread->pDialog.setEndCount(maxCount);
    importMuThread->endCount = maxCount;
    importMuThread->endCount2 = maxCount2;
   

    importMuThread->start();
 //   class mu::ImportFateMaps ImportFateMapsMu;
   // connect(&ImportFateMapsMu,SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));

    /*
    ImportFateMapsMu.pool = pool;
    ImportFateMapsMu.myAlgorithms = myAlgorithms;
    ImportFateMapsMu.readFiles("D:\\FateMap");
    */

}

void MetabolicNavigator::on_actionLabel_position_assistent_triggered()
{
    labelPositionAssistent->show();
}

void MetabolicNavigator::on_actionKegg_Enzymes_triggered()
{
    importEnzymesThread->pool = pool;
    importEnzymesThread->myAlgorithms = myAlgorithms;

    unsigned int maxCount = 0,maxCount2=0;
    QFile file;
    QDir::setCurrent("C:/kegg/ligand/enzyme/");

    file.setFileName("enzyme");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open enzyme");
        return;
    }
    importEnzymesThread->bytes = file.readAll().constData();
    file.close();

    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importEnzymesThread->bytes.find("ENTRY",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }


    importEnzymesThread->pDialog.setEndCount(maxCount);
    importEnzymesThread->endCount = maxCount;
   
    QDir::setCurrent("D:/fatemap");
    file.setFileName("org_list.html");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open org_list.html");
        return;
    }
    importEnzymesThread->bytes2 = file.readAll().constData();
    file.close();


    importEnzymesThread->start();

}

void MetabolicNavigator::on_action_Palson_Kegg_converter_triggered()
{
    palssonKeggConverter->show();
}

void MetabolicNavigator::on_action_Organism_triggered()
{
    importOrganismThread->pool = pool;
    importOrganismThread->myAlgorithms = myAlgorithms;

    unsigned int maxCount = 0,maxCount2=0;
    QFile file;
    //QDir::setCurrent("C:/kegg/ligand/enzyme/");

    QString fileName = QFileDialog::getOpenFileName();
    if(fileName == "")
        return;

    file.setFileName(fileName.toAscii());
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open organism");
        return;
    }
    importOrganismThread->bytes = file.readAll().constData();
    file.close();

    importOrganismThread->start();

}