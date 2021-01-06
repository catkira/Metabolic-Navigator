#ifndef METABOLICNAVIGATOR_H
#define METABOLICNAVIGATOR_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>
#include "ui_metabolicnavigator.h"
#include "importuserdatasets.h"
#include "UserDataset.h"
#include "browsedatasets.h"
#include "importkegg.h"
//#include "FindReactions.h"
//#include "FindCompounds.h"
#include "startupoptions.h"
#include "Options.h"
#include "scopecheck.h"
#include "calculatelinkedreactions.h"
//#include "metabolicwalker.h"
#include "pathfinder.h"
#include "boost/shared_ptr.hpp"
#include "metabolicpathviewer.h"
#include "about.h"
#include "svgviewer/svgviewer.h"
#include "searchdialog.h"
//#include "browseDialog.h"
#include "defines.h"
#include "ImportThread.h"
#include "palssonkeggconverter.h"

class CommonsWidget;
class BrowseCompoundsWidget;
class BrowseReactionsWidget;
class BrowseOrganismsWidget;
class Pool;
class MyAlgorithms;
class LogWidget;
class LabelPositionAssistent;
class PalssonKeggConverter;

class MetabolicNavigator : public QMainWindow
{
    Q_OBJECT

public:
    MetabolicNavigator(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MetabolicNavigator();


private:
    void setupDirs(void);
    Ui::MetabolicNavigatorClass ui;
    LogWidget *logWidget;
    class ImportUserDatasets *importUserDatasets;
    class BrowseDatasets *browseDatasets;
    class ImportKegg *importKegg;
    //class FindReactions *findReactions;
    //class FindCompounds *findCompounds;
    class StartupOptions *startupOptions;
    class Options *options;
    class ScopeCheck *scopeCheck;
    MyAlgorithms *myAlgorithms;
    class CalculateLinkedReactions *calculateLinkedReactions;
    //class MetabolicWalker *metabolicWalker;
    class PathFinder *pathFinder;
    class MetabolicPathViewer *metabolicPathViewer;
    class About *about;
    Pool *pool;
    class SvgViewer *svgViewer;
    class SearchDialog *searchDialog;
    class PalssonKeggConverter *palssonKeggConverter;
    LabelPositionAssistent *labelPositionAssistent;
    BrowseCompoundsWidget *browseCompoundsWidget;
    BrowseReactionsWidget *browseReactionsWidget;
    BrowseOrganismsWidget *browseOrganismsWidget;
    CommonsWidget *commonsWidget;
    ImportThread *importMuThread;
    ImportThread *importEnzymesThread;
    ImportThread *importOrganismThread;

private slots:
    void on_action_Organism_triggered();
    void on_action_Palson_Kegg_converter_triggered();
    void on_actionKegg_Enzymes_triggered();
    void on_actionLabel_position_assistent_triggered();
    void on_actionCarbon_Fate_Maps_triggered();
    void on_actionSBML_2_triggered();
    void on_action_Organisms_triggered();
    void on_actionCommon_actions_triggered();
    void on_actionReactions_2_triggered();
    void on_actionCompounds_Widget_triggered();
    void on_action_Search_window_triggered();
    void on_action_Graph_viewer_triggered();
    void on_actionMetabolic_walker_triggered();
    void on_action_About_triggered();
    void on_actionCalculate_missing_rpairs_triggered();
    void on_actionFix_compartments_triggered();
    void on_action_Path_finder_triggered();
    void on_action_Show_triggered();
    void on_actionCalculate_linked_reactions_for_compounds_triggered();
    void on_actionFind_non_standard_metabolit_names_triggered();
    void on_action_Scope_analysis_triggered();
    void on_action_User_datasets_triggered();
    void on_action_Log_window_triggered();
    void on_action_Startup_triggered();
    void on_actionKegg_triggered();
    void on_actionDatasets_triggered();
    void on_actionCharit_user_files_triggered();
    void on_action_Quit_triggered();
    void closeEvent(QCloseEvent*);
    void commonAction(QString action);
    void requestUpdateSlot(unsigned int signal);
    void showSelectedCompoundsSlot(void);
    void showSelectedReactionsSlot(void);
    void keggImportFinished(void);

signals:
    void addLogLine(QString);
    void update(unsigned int);
};

#endif // METABOLICNAVIGATOR_H
