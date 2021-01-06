#include "importuserdatasets.h"
#include "metabolicnavigator.h"
#include <QDir>
#include "MyAlgorithms.h"
#include "MetaboliteObject.h"
#include "pool.h"
#include "UserDataset.h"
#include "Equation.h"
#include "Reaction.h"
#include "Organism.h"
#include "defines.h"

using namespace std;

ImportUserDatasets::ImportUserDatasets(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    url = new QUrl();
    http = new QHttp();
    readingUserFile=false;

    ui.checkBox_3->setCheckState(Qt::PartiallyChecked);
    ui.checkBox_4->setCheckState(Qt::PartiallyChecked);

    connect(http,SIGNAL(requestFinished(int, bool)),this,SLOT(requestFinished(int, bool)));
    checkForDatasetFiles();
}

ImportUserDatasets::~ImportUserDatasets()
{

}

void ImportUserDatasets::checkForDatasetFiles(void)
{
    QDir dir("C:/");
    QStringList filters;
    QStringList files;
    filters << "*.datasets";
    dir.setPath("C:/datasets/");
    dir.setNameFilters(filters);
    files = dir.entryList();
    emit addLogLine(QString().setNum(files.size()) + " dataset files available");
    ui.listWidget_2->clear();
    for(unsigned int n=0;n<files.size();n++)
        ui.listWidget_2->addItem(files[n]);
}

void ImportUserDatasets::on_pushButton_clicked()
{
    QList<QListWidgetItem *> selectedItems;
    selectedItems = ui.listWidget->selectedItems();
    if(selectedItems.size()==0)
        return;
    UserFile uf;
    userFiles.clear();
    readingUserFile=true;
    userFileRequestId.clear();
    int id;
    userFilesRead=selectedItems.size();
    QDir dir("C:/");
    dir.mkdir("datasets");
    for(unsigned int n=0;n<selectedItems.size();n++)
    {
        emit addLogLine(QString("reading file ")+ selectedItems[n]->text());
        uf.fileName = selectedItems[n]->text();
        uf.fileName.replace(":","-");
        uf.file = new QFile();
        uf.file->setFileName(uf.fileName);
        uf.len = 0;
        userFiles[n]=uf;
        if(QDir::setCurrent("C:/datasets")==false)
            emit addLogLine("could not change path");
        if(uf.file->open(QIODevice::ReadWrite)==false)
            emit addLogLine("can not open file "+ uf.fileName);
        id=http->get(url->path() + selectedItems[n]->text(),uf.file);
        userFileRequestId[id]=n;
    }
}

bool ImportUserDatasets::readUserFile(int id)
{
    emit addLogLine("successfully read "+userFiles[userFileRequestId[id]].fileName);
    userFiles[userFileRequestId[id]].file->flush(); 
    userFiles[userFileRequestId[id]].file->close();
    userFiles[userFileRequestId[id]].bytes = http->readAll();
    userFilesRead--;
    if(userFilesRead==0)
    {
        readingUserFile=false;

        //emit addLogLine("parsing user files");
        //parseUserFiles();
    }
    return true;
}

bool ImportUserDatasets::parseUserFiles(void)
{
    QStringList list;
    class UserDataset userDataset;
    unsigned int numDatasets=0;
    pool->userDatasets->clear();
    //unsigned int reactionIndex=pool->reactions->size();
    unsigned int reactionIndex = 0;
    string reactionID;
    for(unsigned int n=0;n<userFiles.size();n++)
    {   
        emit addLogLine("parsing "+userFiles[n].fileName);
        userFiles[n].bytes.append('\0');
        list = ((QString)userFiles[n].bytes).split('\n');
        for(unsigned int i=1;i<list.size();i++)
        {
            if((list[i])[0]=='#' || list[i].length()<10)
                continue;
            (*(pool->userDatasets))[numDatasets].accepted = getAccepted(list[i]);
            currentAccepted=(*pool->userDatasets)[numDatasets].accepted;
            (*pool->userDatasets)[numDatasets].scientist= getScientist(list[i]);
            (*pool->userDatasets)[numDatasets].keggID = getKeggID(list[i]);
            (*pool->userDatasets)[numDatasets].equation = getEquation(list[i]);
            (*pool->userDatasets)[numDatasets].comment= getComment(list[i]);
            (*pool->userDatasets)[numDatasets].confidence= getConfidence(list[i]);
            (*pool->userDatasets)[numDatasets].equation->globalCompartments = getCompartments(list[i]);
            /*
            if((*pool->userDatasets)[numDatasets].equation->compartments.size()==0 && 
                (*pool->userDatasets)[numDatasets].equation->globalCompartments.size()==0)
            {
                if(ui.checkBox_4->checkState()==Qt::Checked || (ui.checkBox_4->checkState()==Qt::PartiallyChecked && currentAccepted==true))
                    emit addLogLine(QString("reaction ")+(*pool->userDatasets)[numDatasets].keggID.c_str() +" has neither global nor compound compartements");
            }
            */
            (*pool->userDatasets)[numDatasets].excluded = getExcluded(list[i]);

            // create Reaction
            reactionID = string("user") + string(QString().setNum(reactionIndex).toAscii());
            (*pool->reactions)[reactionID].equation = (*pool->userDatasets)[numDatasets].equation;
            (*pool->reactions)[reactionID].id = (*pool->userDatasets)[numDatasets].keggID;
            (*pool->reactions)[reactionID].source = (*pool->userDatasets)[numDatasets].scientist;
            //(*reactions)[reactionIndex].equation->globalCompartments = (*userDatasets)[numDatasets].globalCompartments;
            (*pool->reactions)[reactionID].dataset = numDatasets;
            if((*pool->reactions)[reactionID].id[0] == 'T')
                (*pool->reactions)[reactionID].isTransporter=true;
            else
                (*pool->reactions)[reactionID].isTransporter=false;
            (*pool->userDatasets)[numDatasets].isTransporter = (*pool->reactions)[reactionID].isTransporter;
            //userDatasets[numDatasets] = UserDataset;
            reactionIndex++;
            numDatasets++;
        }
    }
    emit addLogLine(QString().setNum(numDatasets) + " datasets parsed");
    emit newDatasetsAvailable();
    emit updateCompartmentList();
    if(ui.checkBox_5->isChecked())
    {
        myAlgorithms->fixCompartments();
        myAlgorithms->addRpairsFromKeggToUserReactions();
        myAlgorithms->calculateMissingRpairs();
    }
    return true;
}

string ImportUserDatasets::getScientist(QString line)
{
    int val;
    val = line.indexOf((char)0x09,0);
    QString ret = line.mid(0,val);
    return string(ret.toLatin1());
}

string ImportUserDatasets::getKeggID(QString line)
{
    int val,val2;
    val = line.indexOf((char)0x09,0);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    return string(ret.toLatin1());
    
}

boost::shared_ptr<Equation> ImportUserDatasets::getEquation(QString line)
{
    int val,val2;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    bool valid;
    boost::shared_ptr<Equation> eq= (boost::shared_ptr<Equation>)new Equation(myAlgorithms, string(ret.toLatin1()),&valid,pool->availableCompartments, pool->metabolitesPool);
    if(!valid)
        if(ui.checkBox_3->checkState()==Qt::Checked || (ui.checkBox_3->checkState()==Qt::PartiallyChecked && currentAccepted==true))
            emit addLogLine(eq->getLastError().c_str());
    return eq;
}

string ImportUserDatasets::getComment(QString line)
{
    int val,val2;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    return string(ret.toLatin1());
}

int ImportUserDatasets::getConfidence(QString line)
{
    int val,val2;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString buf = line.mid(val+1,1);
    return buf.toInt();
}


set<string> ImportUserDatasets::getCompartments(QString line)
{
    int val,val2;
    set<string> globalCompartments;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    QStringList list = ret.split('+');
    string temp;
    for(unsigned int n=0;n<list.size();n++)
        if(list[n].length()!=0 && list[n]!="none")
        {
            temp = list[n].toLatin1();
            globalCompartments.insert(temp);
            pool->availableCompartments->insert(temp);
        }
    return globalCompartments;;
}

bool ImportUserDatasets::getAccepted(QString line)
{
    int val,val2;
    map<unsigned int, string> globalCompartments;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    if(ret=="true")
        return true;
    else if(ret=="false")
        return false;
    else
        if(ui.checkBox->isChecked())
            emit addLogLine("no accepted information in dataset "+ line);
    return false;
}

bool ImportUserDatasets::getExcluded(QString line)
{
    int val,val2;
    map<unsigned int, string> globalCompartments;
    val = line.indexOf((char)0x09,0);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val = line.indexOf((char)0x09,val+1);
    val2 = line.indexOf((char)0x09,val+1);
    QString ret = line.mid(val+1,val2-val-1);
    if(ret=="true")
        return true;
    else if(ret=="false")
        return false;
    else
        if(ui.checkBox_2->isChecked())
            emit addLogLine("no excluded information in dataset "+ line);
    return true;}


void ImportUserDatasets::on_pushButton_2_clicked()
{
    id_get = 0xFFFFFFFF;
    url->setUrl(ui.lineEdit->text());
    if(url->isValid() == false)
    {
        emit addLogLine("URL is not valid");
        return;
    }
    emit addLogLine("opening URL");
    http->setHost(url->host());
    id_get = http->get(url->path());
}

void ImportUserDatasets::on_lineEdit_textChanged(const QString &)
{

}

void ImportUserDatasets::requestFinished(int id, bool error)
{
    if(readingUserFile)
        readUserFile(id);
    if(id == id_get)
    {
        if(error == true)
        {
            emit addLogLine("error requesting content of directory");
            return;
        }
        emit addLogLine("directory content available");

        QByteArray contentList;
        contentList = http->readAll();
        parseDirectoryContent(contentList);
    }
}

void ImportUserDatasets::parseDirectoryContent(QByteArray contentList)
{
    QString name;
    QString url;
    ui.listWidget->clear();
    getNextHRefPosition=contentList.indexOf("datasets</a>",0);
    while(getNextHRef(&contentList, &name, &url)==true)
    {
        ui.listWidget->addItem(name);
    }
}

bool ImportUserDatasets::getNextHRef(QByteArray *contentList, QString *name, QString *url)
{
    unsigned int val,val2;
    val=contentList->indexOf(QString("<a href=\""),getNextHRefPosition);
    if(val == -1)
        return false;
    val += 9;
    val2=contentList->indexOf("\">",val);
    if(val2==-1)
        return false;
    val2 -= 2;
    getNextHRefPosition = val2;
    QByteArray bytes;
    for(unsigned int n=0;n<val2-val;n++)
    {
        bytes.append((*contentList)[val+n]);
        if((val+n)>= contentList->size())
            return false;
    }
    bytes.append('\0');
    *url = bytes.data();

    val = val2+4;
    val2 = contentList->indexOf("</a>",val);
    if(val2 == -1)
        return false;
    bytes.clear();
    for(unsigned int n=0;n<val2-val;n++)
    {
        bytes.append((*contentList)[val+n]);
        if((val+n)>= contentList->size())
            return false;
    }
    bytes.append('\0');
    *name = bytes.data();

    getNextHRefPosition = val2;

    return true;
}

bool ImportUserDatasets::readFileIntoMem(QString fileName)
{
    return true;
}


void ImportUserDatasets::on_pushButton_4_clicked()
{
    checkForDatasetFiles();
}

void ImportUserDatasets::on_pushButton_3_clicked()
{
    QList<QListWidgetItem *> selectedItems;
    selectedItems = ui.listWidget_2->selectedItems();
    if(selectedItems.size()==0)
        return;
    UserFile uf;
    userFiles.clear();

    userFilesRead=selectedItems.size();
 
    for(unsigned int n=0;n<selectedItems.size();n++)
    {
        emit addLogLine(QString("reading file ")+ selectedItems[n]->text());
        uf.fileName = selectedItems[n]->text();
        uf.fileName.replace(":","-");
        uf.file = new QFile();
        uf.file->setFileName(uf.fileName);
        uf.len = 0;
        if(QDir::setCurrent("C:/datasets")==false)
            emit addLogLine("could not change path");
        if(uf.file->open(QIODevice::ReadOnly)==false)
            emit addLogLine("can not open file "+ uf.fileName);
        uf.bytes = uf.file->readAll();
        uf.file->close();
        userFiles[n]=uf;
        delete uf.file;
    }
    parseUserFiles();
    close();
    generateDefaultOrganisms();
}

void ImportUserDatasets::on_tabWidget_currentChanged(int)
{

}

void ImportUserDatasets::on_pushButton_5_clicked()
{
    importAll();
}

void ImportUserDatasets::importAll(void)
{
    ui.listWidget_2->selectAll();
    on_pushButton_3_clicked();
}

void ImportUserDatasets::generateDefaultOrganisms(void)
{
    Organism organism;
    T_reactionsIterator reactionsIterator;


    organism.setName("User all");    
    reactionsIterator = pool->reactions->begin();
    for(unsigned int n=0;n<pool->reactions->size();n++)
    {
        if(reactionsIterator->second.dataset != -1)
        {
            organism.reactions.insert(&(reactionsIterator->second));
        }
        ++reactionsIterator;
    }

    if(pool->organisms.find(organism.name()) == pool->organisms.end())
        pool->organisms.insert(pair<std::string, Organism>(organism.name(),organism));    


    organism.reactions.clear();
    organism.setName("User accepted only");    
    reactionsIterator = pool->reactions->begin();
    for(unsigned int n=0;n<pool->reactions->size();n++)
    {
        if(reactionsIterator->second.dataset != -1)
        {
            if((*pool->userDatasets)[reactionsIterator->second.dataset].accepted == true)
                organism.reactions.insert(&(reactionsIterator->second));
        }
        ++reactionsIterator;
    }

    if(pool->organisms.find(organism.name()) == pool->organisms.end())
        pool->organisms.insert(pair<std::string, Organism>(organism.name(),organism));    

    emit requestUpdate(UPDATE_ORGANISMS);

}