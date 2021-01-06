#ifndef IMPORTUSERDATASETS_H
#define IMPORTUSERDATASETS_H

#include <QDialog>
#include <QFile>
#include "ui_importuserdatasets.h"
#include <QUrl>
#include <QHttp>
#include <list>
#include <set>
#include "boost/shared_ptr.hpp"


class MetaboliteObject;
class Pool;
class MyAlgorithms;
class Reaction;
class Equation;
class UserDataset;

typedef struct UserFile_
{
    QByteArray bytes;
    QString fileName;
    QFile *file;
    unsigned int len;
} UserFile;

class ImportUserDatasets : public QDialog
{
    Q_OBJECT

public:
    ImportUserDatasets(QWidget *parent = 0);
    ~ImportUserDatasets();
    MyAlgorithms *myAlgorithms;
    void checkForDatasetFiles(void);
    void generateDefaultOrganisms(void);
    void importAll(void);
    class Pool *pool;

private:
    bool getNextHRef(QByteArray *, QString *, QString *);
    bool readFileIntoMem(QString fileName);
    bool readUserFile(int id);
    bool parseUserFiles(void);
    std::string getScientist(QString);
    std::string getKeggID(QString);
    boost::shared_ptr<Equation> getEquation(QString);
    std::string getComment(QString);
    int getConfidence(QString);
    bool getAccepted(QString);
    bool getExcluded(QString);
    std::set<std::string> getCompartments(QString);
    std::map <unsigned int,UserFile> userFiles;

    Ui::ImportUserDatasetsClass ui;
    bool readingUserFile;
    unsigned int userFilesRead;
    std::map<unsigned int, unsigned int> userFileRequestId;
    QUrl *url;
    QHttp *http;
    unsigned int id_get;
    unsigned int getNextHRefPosition;
    bool currentAccepted;

private slots:
    void on_pushButton_5_clicked();
    void on_tabWidget_currentChanged(int);
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_lineEdit_textChanged(const QString &);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void requestFinished(int id, bool error);
    void parseDirectoryContent(QByteArray);

signals:
    void addLogLine(QString);
    void newDatasetsAvailable(void);
    void updateCompartmentList(void);
    void requestUpdate(unsigned int);
};

#endif // IMPORTUSERDATASETS_H
