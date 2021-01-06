#include "ImportSBML.h"
#include <QObject>
#include "Compound.h"
#include "Reaction.h"
#include "Pool.h"
#include "MyAlgorithms.h"
#include "Equation.h"
#include <map>
#include <QDomDocument>
#include <QFile>
#include "Organism.h"

using namespace std;

ImportSBML::ImportSBML(const std::string& fileName, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count) : 
    pool(pool), myAlgorithms(myAlgorithms), fileName(fileName)
{
    QDomDocument doc("SBML File");
    QFile file(fileName.c_str());
    if(!file.open(QIODevice::ReadOnly))
        throw std::exception("can not open sbml file");

    QString error;
    int errorLine, errorColumn;
    if(!doc.setContent(&file, false, &error, &errorLine, &errorColumn))
        throw std::exception(string("can not parse sbml file: " + string(error.toAscii()) ).c_str());

    file.close();


    QDomElement root = doc.documentElement();

    if(root.tagName() != "sbml")
        throw std::exception("file is not a sbml model");

    Organism organism;

    QDomNodeList nodeList = doc.elementsByTagName("species");
    QString temp, temp2, empty;
    QDomElement element;
    Compound* compound;
    map<std::string, std::string> speciesCompartmentMap;
    for(unsigned int n=0;n<nodeList.length();n++)
    {
        element = nodeList.at(n).toElement();
        temp = element.attribute("speciesType",empty);
        if(temp != empty)
        {
            compound = myAlgorithms->getOrCreateCompound(string(temp.toAscii()));
            compound->cNumber = temp.toAscii();
            (*pool->compoundNames)[compound->cNumber]=compound;
            temp = element.attribute("name",empty);
            compound->names.push_back(string(temp.toAscii()));

            pool->availableCompartments->insert(string(element.attribute("compartment",empty).toAscii()));
            //pool->compounds->insert(pair<std::string, Compound>(string(temp.toAscii()), compound));
            
            /*
            speciesCompartmentMap.insert(pair<string,string>
                (string(element.attribute("id",empty).toAscii()),string(element.attribute("name",empty).toAscii())));
                */
        }
    }

    nodeList = doc.elementsByTagName("compartment");
    for(unsigned int n=0;n<nodeList.length();n++)
    {
        element = nodeList.at(n).toElement();
        temp = element.attribute("id",empty);
        temp2 = element.attribute("outside",empty);
        organism.compartments.push_back(string(temp.toAscii()));
        organism.outsideCompartment[string(temp.toAscii())] = string(temp2.toAscii());
    }

    Reaction reaction;
    QDomNode node;
    QDomNodeList nodeList2;
    nodeList = doc.elementsByTagName("reaction");
    string rawEquation;
    set<Reaction*> reactionSet;
    for(unsigned int n=0;n<nodeList.length();n++)
    {
        reaction.clear();
        element = nodeList.at(n).toElement();
        temp = element.attribute("name",empty);
        if(temp == empty)
            throw std::exception("reaction has no name");
        reaction.names.push_back(string(temp.toAscii()));
        reaction.id = string(temp.toAscii());

        node = nodeList.at(n).firstChildElement("listOfReactants");
        if(node.isNull())
            throw std::exception("reaction has no reactants");

        rawEquation.clear();
        nodeList2 = node.childNodes();
        for(unsigned int i=0;i<nodeList2.size();i++)
        {
            element = nodeList2.at(i).toElement();
            temp = element.attribute("species",empty);
            temp2 = element.attribute("stoichiometry",empty);
            temp2.remove(".0");
            rawEquation += " "+string(temp2.toAscii()) + " " + string(temp.toAscii());
        }
        
        rawEquation += " <=> ";
        
        node = nodeList.at(n).firstChildElement("listOfProducts");
        if(node.isNull())
            throw std::exception("reaction has no products");

        nodeList2 = node.childNodes();
        for(unsigned int i=0;i<nodeList2.size();i++)
        {
            element = nodeList2.at(i).toElement();
            temp = element.attribute("species",empty);
            temp2 = element.attribute("stoichiometry",empty);
            temp2.remove(".0");
            rawEquation += " " +string(temp2.toAscii()) + " " + string(temp.toAscii());
        }

        reaction.equation->parseEquation(myAlgorithms, rawEquation,
                    pool->availableCompartments, pool->metabolitesPool);

        unsigned int num = pool->reactions->size();
        string reactionID = "sbml"+ string(QString().setNum(num).toAscii());
        pool->reactions->insert(pair<std::string, Reaction>(reactionID, reaction));
        T_reactionsIterator it = pool->reactions->find(reactionID);
        reactionSet.insert(&(it->second));
    }

    organism.setName("SBML");
    organism.reactions = reactionSet;

    if(pool->organisms.find(organism.name()) == pool->organisms.end())
        pool->organisms.insert(pair<std::string, Organism>(organism.name(),organism));


    /*
    T_reactionsIterator reactionsIterator;
    reactionsIterator = pool->reactions->begin();   
    RPair rpair;
    T_equationMetaboliteIterator equationMetaboliteIterator,equationMetaboliteIterator2;
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.equation->metabolites.size() == 2)
        {
            rpair.leftCompound = reactionsIterator->second.equation->metabolites.at(0).first->compound->cNumber;
            rpair.rightCompound = reactionsIterator->second.equation->metabolites.at(1).first->compound->cNumber;
            rpair.type = "main";
            pool->rpairs.insert(pair<std::string,RPair>
                (
                    reactionsIterator->second.id,
                    rpair
                 ));
            
        }
        else
        {
            equationMetaboliteIterator = reactionsIterator->second.equation->metabolites.begin();
            while(equationMetaboliteIterator != reactionsIterator->second.equation->metabolites.end())
            {
                equationMetaboliteIterator2 = reactionsIterator->second.equation->metabolites.begin();
                while(equationMetaboliteIterator2 != reactionsIterator->second.equation->metabolites.end())
                {
                    if(*equationMetaboliteIterator2 != *equationMetaboliteIterator)
                    {
                        if(equationMetaboliteIterator2->first->compound == equationMetaboliteIterator->first->compound)
                        {
                            rpair.leftCompound = equationMetaboliteIterator->first->compound->cNumber;
                            rpair.rightCompound = equationMetaboliteIterator2->first->compound->cNumber;
                            rpair.type = "main";
                            pool->rpairs.insert(pair<std::string,RPair>
                                (
                                    reactionsIterator->second.id,
                                    rpair
                                 ));
                        }
                    }
                    ++equationMetaboliteIterator2;
                }
                ++equationMetaboliteIterator;
            }
        }

        reactionsIterator++;
    }
    */

}

ImportSBML::~ImportSBML(void)
{
}


void ImportSBML::generateDefaultSBMLOrganisms(string name)
{

}