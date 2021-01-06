#include "importthread.h"
#include <QFile>
#include <QDir>
#include <string>
#include <QTimer>
#include "ImportRPairsFromKegg.h"
#include "ImportReactionsFromKegg.h"
#include "ImportEnzymesFromKegg.h"
#include "ImportOrganism.h"
#include "ImportSBML.h"
#include "ImportFateMapsMu.h"
#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include "compound.h"
#include "reaction.h"
#include "equation.h"
#include "defines.h"
#include "CompoundTransition.h"

using namespace std;
using namespace boost::spirit;

ImportThread::ImportThread(QObject *parent, FunctionType functionType)
    : QThread(parent), functionType_(functionType)
{
    connect(this, SIGNAL(PDshow()), &pDialog, SLOT(show()));
    connect(this, SIGNAL(PDclose()), &pDialog, SLOT(close()));
    connect(this, SIGNAL(PDsetInfoText(QString)), &pDialog, SLOT(setInfoText(QString)));
    connect(this, SIGNAL(PDsetTitle(QString)), &pDialog, SLOT(setTitle(QString)));
    connect(this, SIGNAL(PDsetCurrentCount(unsigned int)), &pDialog, SLOT(setCurrentCount(unsigned int)));
    connect(this, SIGNAL(PDsetEndCount(unsigned int)), &pDialog, SLOT(setEndCount(unsigned int)));

    connect(this, SIGNAL(startUpdateTimer()), &updateTimer, SLOT(start()));
    connect(this, SIGNAL(stopUpdateTimer()), &updateTimer, SLOT(stop()));
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateCount()));

    updateTimer.setInterval(100);
}

ImportThread::~ImportThread()
{
}

void ImportThread::updateCount(void)
{
    pDialog.setCurrentCount(count);
}


void ImportThread::run()
{
    emit PDshow();
    count = 0;
    emit startUpdateTimer();
    switch(functionType_)
    {
    case ITF_COMPOUND:
        emit PDsetTitle("Importing compounds");
        importCompounds();
        //myAlgorithms->validateLinkedRPairs();
        myAlgorithms->validateLinkedReactions();
        break;

    case ITF_REACTION:
        emit PDsetTitle("Importing reactions");
        importReactions();
        myAlgorithms->validateLinkedRPairs();
        myAlgorithms->validateLinkedReactions();
        break;

    case ITF_RPAIR:
        emit PDsetTitle("Importing RPairs");
        importRPairs();
        break;

    case ITF_SBML:
        emit PDsetTitle("Importing SBML");
        try
        {
            importSBML();
        }
        catch(std::exception &e)
        {
            emit addLogLine(e.what());
        }
        myAlgorithms->validateLinkedReactions();
        break;
    case ITF_MU:
        emit PDsetTitle("Importing mu fatemaps");
        try
        {
            importMu();
            myAlgorithms->validateLinkedReactions();
        }
        catch(std::exception &e)
        {
            emit addLogLine(e.what());
        }
        //myAlgorithms->validateLinkedReactions();
        break;
    case ITF_ENZYMES:
        emit PDsetTitle("Importing enzymes");
        try
        {
            importEnzymes();
        }
        catch(std::exception &e)
        {
            emit addLogLine(e.what());
        }
        break;
    case ITF_ORGANISM:
        try
        {
            importOrganism();
        }
        catch(std::exception &e)
        {
            emit addLogLine(e.what());
        }
        break;
    }
    emit stopUpdateTimer();
    emit PDclose();
    emit finished();
}

void ImportThread::importOrganism(void)
{
    emit PDsetEndCount(endCount);
    emit PDsetInfoText("Import Organism");

    
    emit PDsetInfoText("Reading data...");
    ImportOrganism *importOrganism = new ImportOrganism(bytes, pool, myAlgorithms, count);


    delete importOrganism;
    myAlgorithms->setLinkedReactionsDirty();

    emit requestUpdate(UPDATE_ORGANISMS);
}

void ImportThread::importEnzymes(void)
{
    emit PDsetEndCount(endCount);
    emit PDsetInfoText("Reading KEGG enzymes database...");

    
    ImportEnzymesFromKegg *importEnzymesFromKegg = new ImportEnzymesFromKegg(bytes, pool, myAlgorithms, count);

    emit PDsetInfoText("Reading KEGG enzymes database...");
    importEnzymesFromKegg->parseOrganismNames(bytes2);

    emit PDsetInfoText("Creating organisms...");
    importEnzymesFromKegg->buildOrganisms();

    delete importEnzymesFromKegg;

    emit requestUpdate(UPDATE_ORGANISMS);

}

void ImportThread::importCompounds(void)
{
   unsigned int numNames=0;
    unsigned int lineEnd=0;
    lineEnd = bytes.find('\n',0);
    unsigned int lineStart=0;
    string tempString;
    unsigned int nextEntry;
    string cNumber;
    unsigned int bytesSize = bytes.size();
    count = 0;
    unsigned int val = 0;
    unsigned int numCompounds = 0;
    log.clear();
    do
    {
        val = bytes.find("ENTRY",lineStart);
        nextEntry = bytes.find("ENTRY",val+1);
        if(nextEntry==string::npos)
            nextEntry = bytes.size();
        if(val!=string::npos)
        {
            cNumber = bytes.substr(val+12,6);
            /*
            pool->compounds->insert(map<string, Compound>::value_type(cNumber,Compound()));
            */
            myAlgorithms->getOrCreateCompound(cNumber);
            compoundsIterator = pool->compounds->find(cNumber);
            compoundsIterator->second.cNumber = cNumber;
            (*pool->compoundNames)[compoundsIterator->second.cNumber]=&(compoundsIterator->second);
            numCompounds++;

            val = bytes.find("NAME",lineStart);
            if(val==string::npos || val > nextEntry)
            {            
                emit addLogLine(string(string("no name found for compound ") + compoundsIterator->second.cNumber).c_str());
                lineStart =  bytes.find("\n",lineStart);
            }
            else
            {
                lineEnd = bytes.find('\n',val);
                if(lineEnd == string::npos)
                    lineEnd = bytes.size() -1;
                numNames=0;
                lineStart = val;
                do{
                    if(bytes[lineEnd-1]==';')
                        compoundsIterator->second.names.push_back(bytes.substr(lineStart+12,lineEnd-(lineStart+12)-1));
                        //compoundsIterator->second.names[numNames++] = bytes.substr(lineStart+12,lineEnd-(lineStart+12)-1);
                    else
                        compoundsIterator->second.names.push_back(bytes.substr(lineStart+12,lineEnd-(lineStart+12)));
                        //compoundsIterator->second.names[numNames++] = bytes.substr(lineStart+12,lineEnd-(lineStart+12));
                    // all the names
                    lineStart = lineEnd+1;
                    lineEnd = bytes.find('\n',lineStart);
                    if(lineEnd == string::npos)
                        lineEnd = bytes.size() -1;                
                }while(bytes[lineStart]==' ' && lineStart < (bytes.size()-1));
            }

            val = bytes.find("FORMULA",lineStart);
            if(val==string::npos || val > nextEntry)
            {            
                lineStart =  bytes.find("\n",lineStart);
                //emit addLogLine(QString("no formula found for compound ")+(*compounds)[numCompounds-1].cNumber.c_str());
            }
            else
            {
                lineEnd = bytes.find('\n',val);
                tempString = bytes.substr(lineStart+12,lineEnd - lineStart - 12);
                //tempString[lineEnd-val-12]='\0';
                //emit addLogLine(tempString.c_str());
                compoundsIterator->second.formula.parseFormula(tempString);
                if(compoundsIterator->second.formula.isValid() == false)
                    //throw std::exception(compoundsIterator->second.formula.getLastError().c_str());
                    //log.push_back(compoundsIterator->second.formula.getLastError());
                    emit addLogLine(compoundsIterator->second.formula.getLastError().c_str());
            }

            val = bytes.find("Same as: ",lineStart);
            if(val!=string::npos && val < nextEntry)
            {
                val += 9;
                lineEnd = bytes.find('\n',val);
                unsigned int pos2=val;
                unsigned int tempval;
                string tempstring;
                do
                {
                    tempval = bytes.find(" ",pos2);
                    if(tempval == string::npos || tempval > lineEnd)
                        tempval = lineEnd;
                    tempstring = bytes.substr(pos2,tempval-pos2);
                    compoundsIterator->second.sameAs.insert(tempstring);
                    pos2= tempval+1;
                }while(pos2 < lineEnd);
                
            }
        }
        ++count;
    }while(nextEntry < bytesSize);
    
    myAlgorithms->setLinkedReactionsDirty();

    emit PDsetInfoText("Updating compound list...");
    emit requestUpdate(UPDATE_COMPOUNDS);
}

string getKeggValue(string *bytes, unsigned int start)
{
    unsigned int nextLineStart,lineStart;
    lineStart = start;
    string ret;
    do
    {
        nextLineStart = bytes->find('\n',lineStart)+1;    
        ret += bytes->substr(lineStart+12, nextLineStart-1 - lineStart - 12);
        lineStart = nextLineStart;
        if((*bytes)[lineStart]==' ')
            ret += " ";
    }while((*bytes)[lineStart]==' ');
    return ret;
}

void ImportThread::importReactions(void)
{
    unsigned int numNames=0;
    unsigned int lineEnd=0;
    unsigned int lineStart=0;
    unsigned int nextEntry;
    unsigned int bytesSize = bytes.size();
    unsigned int val = 0;
    count = 0;
    unsigned int numReactions = pool->reactions->size();
    unsigned int oldNumReactions = pool->reactions->size();
    T_reactionsIterator reactionsIterator;
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;
    uint_parser<unsigned, 10, 5, 5> uint5_p;
    //RPair rpair;
    CompoundTransition compoundTransition;
    string reactionID;

    T_compoundsIterator compoundIterator;

    do
    {
        val = bytes.find("ENTRY",lineStart);
        nextEntry = bytes.find("ENTRY",val+1);
        if(nextEntry==string::npos)
            nextEntry = bytes.size();
        if(val!=string::npos)
        {
            reactionID = bytes.substr(val+12,6);
            pool->reactions->insert(map<std::string,Reaction>::value_type(reactionID,Reaction()));
            reactionsIterator = pool->reactions->find(reactionID);
            numReactions++;
            reactionsIterator->second.dataset = -1;
            reactionsIterator->second.source = string("kegg");
            reactionsIterator->second.id = reactionID;
            //reactionsIterator->second.id = bytes.substr(val+12,6);
        
            val = bytes.find("NAME",lineStart);
            if(val==string::npos || val > nextEntry)
            {            
                lineStart =  bytes.find("\n",lineStart);
                emit addLogLine(QString("no name found for reaction ")+reactionsIterator->second.id.c_str());
                //continue;
            }
            else
            {
                lineEnd = bytes.find('\n',val);
                if(lineEnd == string::npos)
                    lineEnd = bytes.size() -1;
                numNames=0;
                lineStart = val;
                do{
                    if(bytes[lineEnd-1]==';')
                        reactionsIterator->second.names.push_back(bytes.substr(lineStart+12,lineEnd-(lineStart+12)-1));
                        //reactionsIterator->second.names[numNames++] = bytes.substr(lineStart+12,lineEnd-(lineStart+12)-1);
                    else
                        reactionsIterator->second.names.push_back(bytes.substr(lineStart+12,lineEnd-(lineStart+12)));
                        //reactionsIterator->second.names[numNames++] = bytes.substr(lineStart+12,lineEnd-(lineStart+12));
                    // all the names
                    lineStart = lineEnd+1;
                    lineEnd = bytes.find('\n',lineStart);
                    if(lineEnd == string::npos)
                        lineEnd = bytes.size() -1;                
                }while(bytes[lineStart]==' ' && lineStart < (bytes.size()-1));
            }
      
            val = bytes.find("EQUATION",lineStart);
            if(val==string::npos  || val > nextEntry)
            {            
                lineStart =  bytes.find("\n",lineStart);
                emit addLogLine(QString("no equation found for reaction ")+reactionsIterator->second.id.c_str());
            }
            else
            {
                lineStart = val;
                lineEnd = bytes.find('\n',lineStart);
                reactionsIterator->second.equation->parseEquation(myAlgorithms, getKeggValue(&bytes, lineStart),
                    pool->availableCompartments, pool->metabolitesPool);
            }

            val = bytes.find("RPAIR",lineStart);
            if(val==string::npos  || val > nextEntry)
            {
                lineStart =  bytes.find("\n",lineStart);
            }
            else
            {
                /* too slow
                t_rule SIXCHARS = (anychar_p - space_p - eol_p)
                    >> (anychar_p - space_p - eol_p)
                    >> (anychar_p - space_p - eol_p)
                    >> (anychar_p - space_p - eol_p)
                    >> (anychar_p - space_p - eol_p)
                    >> (anychar_p - space_p - eol_p);
                t_rule RPAIRS = 
                    str_p("RPAIR")>>
                    
                    +(
                        *space_p >>
                        str_p("RP:") >>
                        +space_p >>
                        SIXCHARS >>
                        +space_p >>
                        SIXCHARS[assign_a(rpair.leftCompound)] >> ch_p('_') >> SIXCHARS[assign_a(rpair.rightCompound)] >>
                        +space_p >>
                        (+(anychar_p - eol_p))[assign_a(rpair.type)] >> eol_p
                    )[insert_at_a(pool->rpairs, reactionsIterator->second.id, rpair)];
                    */

                //parse((bytes.c_str() + val), RPAIRS);

                
                val = bytes.find("RP:",val);
                while(val!=string::npos && val < nextEntry)
                {
                    lineStart=val;
                    /*
                    reactionsIterator->second.rpair.insert(pair<pair<string,string>,string>
                        (pair<string,string>(bytes.substr(val+12,6),bytes.substr(val+19,6)),bytes.substr(val+26,4)));
                        */
                    /*
                    rpair.leftCompound = bytes.substr(val+12,6);
                    rpair.rightCompound = bytes.substr(val+19,6);
                    rpair.type = bytes.substr(val+26,4);
                    pool->rpairs.insert(pair<std::string, RPair>(reactionsIterator->second.id, rpair));
                    */
                    compoundTransition.compoundPairID.first = bytes.substr(val+12,6);
                    compoundTransition.compoundPairID.second = bytes.substr(val+19,6);

                    compoundIterator = pool->compounds->find(compoundTransition.compoundPairID.first);
                    if(compoundIterator != pool->compounds->end())
                        compoundTransition.compoundPair.first = &compoundIterator->second;
                    compoundIterator = pool->compounds->find(compoundTransition.compoundPairID.second);
                    if(compoundIterator != pool->compounds->end())
                        compoundTransition.compoundPair.second = &compoundIterator->second;

                    compoundTransition.type = bytes.substr(val+26,4);
                    compoundTransition.inReactionsID.clear();
                    compoundTransition.inReactionsID.push_back(reactionsIterator->second.id);
                    pool->compoundTransitions.insert(pair<std::string, CompoundTransition>(reactionsIterator->second.id, compoundTransition));

                    val = bytes.find("RP:",val+1);
                }
                
            }

        }
        ++count;
    }while(nextEntry < bytesSize);
    emit addLogLine(QString().setNum(numReactions - oldNumReactions) + " reactions read");

    string temp="";
    ImportReactionsFromKegg importReactionsFromKegg(temp, pool, myAlgorithms, count);
    importReactionsFromKegg.generateDefaultKeggOrganisms();

    myAlgorithms->setLinkedReactionsDirty();
    emit requestUpdate(UPDATE_ORGANISMS);
    emit PDsetInfoText("Updating reaction list...");

    emit requestUpdate(UPDATE_REACTIONS);

}

void ImportThread::importRPairs(void)
{
    //pDialog.setInfoText("Parsing RPairs...");
    emit PDsetInfoText("Parsing RPairs...");
    ImportRPairsFromKegg(bytes, pool, myAlgorithms, count);
}


void ImportThread::importSBML(void)
{
    ImportSBML(bytes, pool, myAlgorithms, count);
    emit requestUpdate(UPDATE_ORGANISMS);
    emit requestUpdate(UPDATE_AVAILABLE_COMPARTMENTS);
}

void ImportThread::importMu(void)
{
    //ImportSBML(bytes, pool, myAlgorithms, count);
    mu::ImportFateMaps importMu(pool, myAlgorithms, count);
    //importMu.readFiles(compoundBytes, reactionBytes);
    pDialog.setEndCount(endCount);
    emit PDsetInfoText("Reading compounds...");
    importMu.readCompounds(compoundBytes);


    count = 0;
    //pDialog.setCurrentCount(0);
    //pDialog.setEndCount(endCount2);
    emit PDsetEndCount(endCount2);
    emit PDsetInfoText("Reading reactions and calculating matrices...");
    importMu.readReactions(reactionBytes);
    compoundBytes.clear();
    reactionBytes.clear();

    myAlgorithms->setLinkedReactionsDirty();

    emit requestUpdate(UPDATE_ORGANISMS);
    emit requestUpdate(UPDATE_COMPOUNDS);
    emit requestUpdate(UPDATE_REACTIONS);
    //emit requestUpdate(UPDATE_AVAILABLE_COMPARTMENTS);
}