#include "MyAlgorithms.h"
#include <algorithm>
#include <set>
#include "Helper.h"
#include <map>
#include "userdataset.h"
#include "equation.h"
#include "TreeNode.h"
#include "ConnectionObject.h"
#include "MetabolicTree.h"
#include "CompoundTransition.h"

#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>

using namespace std;
using namespace boost::spirit;

MyAlgorithms::MyAlgorithms(void)
{
    linkedReactionsDirty = true;
}

MyAlgorithms::~MyAlgorithms(void)
{
}

void MyAlgorithms::triggerAddLogLineSignal(const std::string& text)
{
    emit addLogLine(text.c_str());
}


class AddReactionSetting
{
public:
    AddReactionSetting(MyAlgorithms& self) : self(self)
    {
    }
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        try
        {
            T_reactionsIterator reactionsIterator_;
            reactionsIterator_ = self.pool->reactions->find(self.tempReactionID);
            if(reactionsIterator_ == self.pool->reactions->end())
            {
                string temp =  "reaction " + self.tempReactionID + " not found";
                throw std::exception(temp.c_str());
            }
            if(self.tempReactionSetting == "BOTH")
                reactionsIterator_->second.userSettings = RS_DIRECTION_BOTH;
            else if(self.tempReactionSetting == "FORWARD")
                reactionsIterator_->second.userSettings = RS_DIRECTION_FORWARD;
            else if(self.tempReactionSetting == "BACKWARD")
                reactionsIterator_->second.userSettings = RS_DIRECTION_BACKWARD;
            else if(self.tempReactionSetting == "NONE")
                reactionsIterator_->second.userSettings = RS_DIRECTION_NONE;
            else
            {
                string temp = self.tempReactionSetting + " is an unknown setting";
                throw std::exception(temp.c_str());
            }

        }   
        catch(std::exception &e)
        {
            string temp = e.what();
            self.triggerAddLogLineSignal(temp);
            return;
        }    
    };

private:
    MyAlgorithms& self;

};

void MyAlgorithms::parseReactionUserSettings(string text)
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    AddReactionSetting addReactionSetting(*this);

    t_rule REACTION_ID = +(anychar_p - space_p - eol_p - ch_p(':'));
    t_rule REACTION_SETTING = +(anychar_p - space_p - eol_p);

    t_rule COMMENT_LINE = 
        str_p("//") 
        >> *(anychar_p - eol_p);
    t_rule REACTIONDEFINITION_LINE = 
        REACTION_ID[assign_a(tempReactionID)] 
        >> ( (*space_p >> ch_p(':') >> *space_p) | ch_p(' '))
        >> REACTION_SETTING[assign_a(tempReactionSetting)]
        >> *space_p
        >> *COMMENT_LINE;

    t_rule MAIN = 
        *(
            (
                COMMENT_LINE | 
                REACTIONDEFINITION_LINE[addReactionSetting]
            )
            >> *eol_p
        );

    
    BOOST_SPIRIT_DEBUG_NODE(REACTION_ID);
    BOOST_SPIRIT_DEBUG_NODE(REACTION_SETTING);
    BOOST_SPIRIT_DEBUG_NODE(COMMENT_LINE);
    BOOST_SPIRIT_DEBUG_NODE(REACTIONDEFINITION_LINE);
    BOOST_SPIRIT_DEBUG_NODE(MAIN);

    // set everything to default
    T_reactionsIterator rIt = pool->reactions->begin();
    while(rIt != pool->reactions->end())
    {
        rIt->second.userSettings = RS_DIRECTION_BOTH;
        ++rIt;
    }

    if(!parse(text.c_str(), MAIN).full)
    {
        emit addLogLine("could not fully parse reaction settings!");
    }

}

void MyAlgorithms::calculateMissingRpairs(void)
{
    //vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    //vector<pair<string, unsigned int>>::iterator equationCompoundsIterator2;

    /*
    map<unsigned int, Reaction>::iterator reactionsIterator;
    reactionsIterator = pool->reactions->begin();   
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.rpair.empty() == false)
        {
            reactionsIterator++;
            continue;
        }


        if(reactionsIterator->second.equation->metabolites.size() == 2)
        {
            reactionsIterator->second.rpair.insert(pair<pair<string,string>,string>
                (pair<string,string>
                (
                    reactionsIterator->second.equation->metabolites.at(0).first->compound->cNumber,
                    reactionsIterator->second.equation->metabolites.at(1).first->compound->cNumber
                 ),"main"));

        }
        reactionsIterator++;
    }
*/
}

void MyAlgorithms::addRpairsFromKeggToUserReactions(void)
{
    /*
    map<string, Reaction*> keggReactionNames;
    map<string, Reaction*>::iterator keggReactionNamesIterator;
    map<unsigned int, Reaction>::iterator reactionsIterator;
    reactionsIterator = pool->reactions->begin();
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.dataset == -1)
            keggReactionNames.insert(pair<string,Reaction*>
                (reactionsIterator->second.id,&(reactionsIterator->second)));
        reactionsIterator++;
    }

    reactionsIterator = pool->reactions->begin();
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.dataset != -1)
        {
            keggReactionNamesIterator = keggReactionNames.find(reactionsIterator->second.id);
            if(keggReactionNamesIterator != keggReactionNames.end())
            {
                 reactionsIterator->second.rpair = keggReactionNamesIterator->second->rpair;
            }
            else
            {
            }
        }
        reactionsIterator++;
    }
    */
}


void MyAlgorithms::fixCompartments(void)
{
    map <string,string>::iterator compartmentsIterator;
    T_equationMetabolitesIterator equationMetabolitesIterator;
    set <string>::iterator globalCompartmentsIterator;
    T_reactionsIterator reactionsIterator;
    while(reactionsIterator != pool->reactions->end())
    {
    //for(unsigned int n=0;n<pool->reactions->size();n++)
    //{
        //compartmentsIterator = (*(pool->reactions))[n].equation->compartments.begin();
        equationMetabolitesIterator = reactionsIterator->second.equation->metabolites.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->metabolites.size();k++)
        {
            if(equationMetabolitesIterator->first.compartment.c_str() == string("mitoIm") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("mitoTm") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("mito"))
            {
                //(*reactions)[n].equation->compartments[k]="mito";
                equationMetabolitesIterator->first.compartment.assign("mito");
            }
            equationMetabolitesIterator++;
        }

        equationMetabolitesIterator = reactionsIterator->second.equation->metabolites.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->metabolites.size();k++)
        {
            if(equationMetabolitesIterator->first.compartment.c_str() == string("mitoOm") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("mitoMx") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("cyto") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("nuc"))
            {
                //(*reactions)[n].equation->compartments[k] = "cyto";
                equationMetabolitesIterator->first.compartment.assign("cyto");
            }
            equationMetabolitesIterator++;
        }

        equationMetabolitesIterator = reactionsIterator->second.equation->metabolites.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->metabolites.size();k++)
        {
            if(equationMetabolitesIterator->first.compartment.c_str() == string("endo") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("cisgol") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("transgol") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("micro") ||
                equationMetabolitesIterator->first.compartment.c_str() == string("golgi"))
            {
                //(*reactions)[n].equation->compartments[k] = "GolErMi";
                equationMetabolitesIterator->first.compartment.assign("GolErMi");
            }
            equationMetabolitesIterator++;
        }


        // -------------- globalCompartments
        globalCompartmentsIterator = reactionsIterator->second.equation->globalCompartments.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->globalCompartments.size();k++)
        {
            if(globalCompartmentsIterator->c_str() == string("mitoIm") ||
                globalCompartmentsIterator->c_str() == string("mitoTm") ||
                globalCompartmentsIterator->c_str() == string("mito"))
            {
                //(*reactions)[n].equation->globalCompartments.erase(globalCompartmentsIterator);
                //(*reactions)[n].equation->globalCompartments.insert("mito");
                globalCompartmentsIterator->assign("mito");
            }
            globalCompartmentsIterator++;
        }

        globalCompartmentsIterator = reactionsIterator->second.equation->globalCompartments.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->globalCompartments.size();k++)
        {
            if(globalCompartmentsIterator->c_str() == string("mitoOm") ||
                globalCompartmentsIterator->c_str() == string("mitoMx") ||
                globalCompartmentsIterator->c_str() == string("cyto") ||
                globalCompartmentsIterator->c_str() == string("nuc"))
            {
                //(*reactions)[n].equation->globalCompartments.erase(globalCompartmentsIterator);
                //(*reactions)[n].equation->globalCompartments.insert("cyto");
                //globalCompartmentsIterator = string("cyto");
                globalCompartmentsIterator->assign("cyto");
            }
            globalCompartmentsIterator++;
        }

        globalCompartmentsIterator = reactionsIterator->second.equation->globalCompartments.begin();
        for(unsigned int k=0;k<reactionsIterator->second.equation->globalCompartments.size();k++)
        {
            if(globalCompartmentsIterator->c_str() == string("endo") ||
                globalCompartmentsIterator->c_str() == string("cisgol") ||
                globalCompartmentsIterator->c_str() == string("transgol") ||
                globalCompartmentsIterator->c_str() == string("micro") ||
                globalCompartmentsIterator->c_str() == string("golgi"))
            {
                //(*reactions)[n].equation->globalCompartments.erase(globalCompartmentsIterator);
                //(*reactions)[n].equation->globalCompartments.insert("GolErMi");
                globalCompartmentsIterator->assign("GolErMi");
            }
            globalCompartmentsIterator++;
        }
        ++reactionsIterator;

    }
    pool->availableCompartments->insert("GolErMi");
    pool->availableCompartments->erase("mitoIm");
    pool->availableCompartments->erase("mitoTm");
    pool->availableCompartments->erase("mitoOm");
    pool->availableCompartments->erase("mitoMx");
    pool->availableCompartments->erase("nuc");
    pool->availableCompartments->erase("endo");
    pool->availableCompartments->erase("cisgol");
    pool->availableCompartments->erase("transgol");
    pool->availableCompartments->erase("micro");
    pool->availableCompartments->erase("golgi");
    pool->availableCompartments->erase("endo");
    emit updateCompartmentList();

}

void MyAlgorithms::addIfCompartmentMatches(const MetaboliteObject& startMetabolite,
            unsigned int startMetabolitePosition,
            //string endMetaboliteCNumber,
            T_equationMetabolite newEquationMetabolite,
            Reaction *reaction,
            map<MetaboliteObject *, vector<Reaction *>> *targetMetaboliteLinkedReactions,
            map<Reaction *, vector<MetaboliteObject*>> *targetReactionLinkedMetabolites,
            bool withCompartments)
{
    MetaboliteObject *metaboliteObject;

    if(withCompartments==true)
    {
        if(reaction->equation->hasMetaboliteCompartments() == true)
        {
            T_equationMetabolitesIterator equationMetabolitesIterator;
            equationMetabolitesIterator = reaction->equation->metabolites.begin();
            bool found=false;
            while(equationMetabolitesIterator != reaction->equation->metabolites.end())
            {
                if(equationMetabolitesIterator->first.compound->cNumber == startMetabolite.compound->cNumber &&
                   (equationMetabolitesIterator->second & 0x110000) == startMetabolitePosition && 
                   equationMetabolitesIterator->first.compartment == startMetabolite.compartment)
                {
                    //if( equationMetabolitesIterator->first->compartment != startMetabolite.compartment)
                        //return;
                    metaboliteObject = getOrCreateMetaboliteObject(newEquationMetabolite.first.compound,
                        newEquationMetabolite.first.compartment);
                    found = true;
                    break;
                }
                equationMetabolitesIterator++;
            }
            if(!found)
                return;
        }
        else if(reaction->equation->globalCompartments.find(startMetabolite.compartment)==
            reaction->equation->globalCompartments.end())
            return;

        else
            metaboliteObject = getOrCreateMetaboliteObject(newEquationMetabolite.first.compound, startMetabolite.compartment);
    }
    else
    {
        metaboliteObject = getOrCreateMetaboliteObject(newEquationMetabolite.first.compound,"",newEquationMetabolite.first.label);
        //metaboliteObject = newEquationMetabolite->first;
    }
    (*targetMetaboliteLinkedReactions)[metaboliteObject].push_back(reaction);
    (*targetReactionLinkedMetabolites)[reaction].push_back(metaboliteObject);
}



void MyAlgorithms::calculatePossibleNewMetabolites(MetaboliteObject const*  startMetabolite, 
    const set<Reaction*>* const reactionSet, 
    map<MetaboliteObject *, vector<Reaction *>> *targetMetaboliteLinkedReactions,
    map<Reaction *, vector<MetaboliteObject*>> *targetReactionLinkedMetabolites,
    bool withCompartments,
    unsigned int rpairsOption,
    bool reverse)
{
    map<string,Compound *>::iterator compoundNamesIterator;
    targetMetaboliteLinkedReactions->clear();
    targetReactionLinkedMetabolites->clear();    
    vector<pair<Reaction*, unsigned int>>::iterator linkedReactionsIterator;
    map<string,MetaboliteObject*>::iterator metabolitesPoolIterator;
    MetaboliteObject *metaboliteObject;
    Reaction *currentReaction;
    unsigned int metabolitePosition;
    vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    T_equationMetabolitesIterator equationMetabolitesIterator;
    //map<pair<string,string>,string>::iterator rpairIterator;
    T_rpairsIterator rpairIterator;
    T_compoundTransitionsIterator compoundTransitionsIterator;
    T_equationMetabolite newEquationMetabolite;
    bool skipRest;
    string tempString;

    validateLinkedReactions();

    linkedReactionsIterator = startMetabolite->compound->linkedReactions.begin();
    while(linkedReactionsIterator != startMetabolite->compound->linkedReactions.end())
    {
        currentReaction = linkedReactionsIterator->first;
        metabolitePosition = linkedReactionsIterator->second;
        linkedReactionsIterator++;
        
        if(reactionSet->find(currentReaction)==reactionSet->end())
            continue;

        if((rpairsOption & 0xFF )>TRANSITIONCONDITION_ANY && pool->compoundTransitions.count(currentReaction->id) != 0)
        {
            /*
            skipRest=false;
            if((rpairsOption & 0xFF00) == TRANSITIONCONDITION_NOPATH)
                skipRest=true;
                */
            //rpairIterator = pool->rpairs.find(currentReaction->id);
            compoundTransitionsIterator = pool->compoundTransitions.find(currentReaction->id);
            //while(rpairIterator != pool->rpairs.end() && rpairIterator->first==currentReaction->id  )
            while(compoundTransitionsIterator != pool->compoundTransitions.end() && compoundTransitionsIterator->first==currentReaction->id  )
            {
                if(compoundTransitionsIterator->second.compoundPairID.first == startMetabolite->compound->cNumber ||
                    compoundTransitionsIterator->second.compoundPairID.second == startMetabolite->compound->cNumber)
                {
                    // rpairFiltering becomes active if startMetabolite is found in rpairs
                    //skipRest=true;


                    if((rpairsOption & 0xFF ) == TRANSITIONCONDITION_LABEL && compoundTransitionsIterator->second.type == string("carb"))
                    {
                        if(startMetabolite->label.size() == 0)  
                            return;
                        if(compoundTransitionsIterator->second.compoundPairID.first == startMetabolite->compound->cNumber)
                            tempString = compoundTransitionsIterator->second.compoundPairID.second;
                        else
                            tempString = compoundTransitionsIterator->second.compoundPairID.first;
                        equationMetabolitesIterator = currentReaction->equation->metabolites.begin();
                        while(equationMetabolitesIterator != currentReaction->equation->metabolites.end())
                        {
                            if((equationMetabolitesIterator->second & 0x110000)== metabolitePosition 
                                ||
                                currentReaction->userSettings == RS_DIRECTION_NONE
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && reverse
                                    )
                                )
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && reverse
                                    )
                                )
                                )
                            {
                                equationMetabolitesIterator++;
                                continue;
                            }
                            if(equationMetabolitesIterator->first.compound->cNumber == tempString)
                            {   
                                newEquationMetabolite.first = equationMetabolitesIterator->first;
                                newEquationMetabolite.second = equationMetabolitesIterator->second;
                                if(compoundTransitionsIterator->second.compoundPairID.first == startMetabolite->compound->cNumber)
                                {
                                    newEquationMetabolite.first.label = 
                                    //equationMetabolitesIterator->first.label = 
                                        boost::numeric::ublas::prod(*compoundTransitionsIterator->second.transitionMatrix,startMetabolite->label);
                                }
                                else
                                {
                                    newEquationMetabolite.first.label = 
                                    //equationMetabolitesIterator->first.label = 
                                        boost::numeric::ublas::prod(
                                            boost::numeric::ublas::trans(*compoundTransitionsIterator->second.transitionMatrix),startMetabolite->label);
                                }

                                if(newEquationMetabolite.first.isLabeled())
                                {

                                    addIfCompartmentMatches(*startMetabolite,
                                        metabolitePosition,
                                        newEquationMetabolite,
                                        currentReaction,
                                        targetMetaboliteLinkedReactions,
                                        targetReactionLinkedMetabolites,
                                        withCompartments);              
                                }
                            }
                            equationMetabolitesIterator++;    
                        }
                    }

                    else if(
                        ((compoundTransitionsIterator->second.type == string("main") || 
                        compoundTransitionsIterator->second.type == string("tran") || 
                        compoundTransitionsIterator->second.type == string("leav")) && (rpairsOption & 0xFF ) == TRANSITIONCONDITION_MAIN)
                        ||
                        compoundTransitionsIterator->second.type == string("carb") && (rpairsOption & 0xFF ) == TRANSITIONCONDITION_CARBON)
                        // startMetabolite is main in current reaction -> add it
                    {
                        if(compoundTransitionsIterator->second.compoundPairID.first == startMetabolite->compound->cNumber)
                            tempString = compoundTransitionsIterator->second.compoundPairID.second;
                        else
                            tempString = compoundTransitionsIterator->second.compoundPairID.first;
                        //bool found=false;
                        equationMetabolitesIterator = currentReaction->equation->metabolites.begin();
                        while(equationMetabolitesIterator != currentReaction->equation->metabolites.end())
                        {
                            newEquationMetabolite.first = equationMetabolitesIterator->first;
                            newEquationMetabolite.second = equationMetabolitesIterator->second;

                            if((equationMetabolitesIterator->second & 0x110000)== metabolitePosition 
                                ||
                                currentReaction->userSettings == RS_DIRECTION_NONE
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && reverse
                                    )
                                )
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && reverse
                                    )
                                )
                                )
                            {
                                equationMetabolitesIterator++;
                                continue;
                            }

                            if(equationMetabolitesIterator->first.compound->cNumber == tempString)
                            {                            
                                addIfCompartmentMatches(*startMetabolite,
                                    metabolitePosition,
                                    newEquationMetabolite,
                                    currentReaction,
                                    targetMetaboliteLinkedReactions,
                                    targetReactionLinkedMetabolites,
                                    withCompartments);                
                            }
                            equationMetabolitesIterator++;    
                        }
                    }
                }
                compoundTransitionsIterator++;
            }
            /*
            if(skipRest==true)
                continue;
                */
        }
        if((rpairsOption & 0xFF00 )  == TRANSITIONCONDITION_NOPATH && (rpairsOption & 0xFF ) != TRANSITIONCONDITION_ANY)
            continue;
        //equationCompoundsIterator = currentReaction->equation->Compounds.begin();
        //for(unsigned int k=0;k<currentReaction->equation->metabolites.size();k++)
        equationMetabolitesIterator = currentReaction->equation->metabolites.begin();
        while(equationMetabolitesIterator != currentReaction->equation->metabolites.end())
        {
            // skip if product candidate is on same side as substrate
                            if((equationMetabolitesIterator->second & 0x110000)== metabolitePosition 
                                ||
                                currentReaction->userSettings == RS_DIRECTION_NONE
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_FORWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && reverse
                                    )
                                )
                                ||
                                (   
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_LEFT) != 0 && !reverse
                                    )
                                    ||
                                    (
                                    currentReaction->userSettings == RS_DIRECTION_BACKWARD && 
                                    (metabolitePosition & METABOLITE_IS_RIGHT) != 0 && reverse
                                    )
                                )
                                )
            {
                equationMetabolitesIterator++;
                continue;
            }
            newEquationMetabolite.first = equationMetabolitesIterator->first;
            newEquationMetabolite.second = equationMetabolitesIterator->second;

            addIfCompartmentMatches(*startMetabolite,
                metabolitePosition,
                newEquationMetabolite,
                currentReaction,
                targetMetaboliteLinkedReactions,
                targetReactionLinkedMetabolites,
                withCompartments);

            equationMetabolitesIterator++;
        }
    }
}

void MyAlgorithms::calculateLinkedRPairs(void)
{
}

void MyAlgorithms::calculateLinkedReactions(void)
{
    unsigned int n;
    map<string, Compound*>::iterator compoundNamesIterator;
    T_reactionsIterator reactionsIterator;

    // clear linked reactions first
    T_compoundsIterator compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        compoundsIterator->second.linkedReactions.clear();
        ++compoundsIterator;
    }


    //vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    vector<T_equationMetabolite>::iterator equationMetabolitesIterator;
    reactionsIterator = pool->reactions->begin();
    //for(unsigned int i=0;i<pool->reactions->size();i++)
    while(reactionsIterator != pool->reactions->end())
    {
        equationMetabolitesIterator = reactionsIterator->second.equation->metabolites.begin();
        while(equationMetabolitesIterator != reactionsIterator->second.equation->metabolites.end())
        {
            if(equationMetabolitesIterator->first.compound->cNumber == "")
            {
                ++equationMetabolitesIterator;
                continue;
            }
            compoundNamesIterator = pool->compoundNames->find(equationMetabolitesIterator->first.compound->cNumber);
            if(compoundNamesIterator != pool->compoundNames->end())
            {
                if((equationMetabolitesIterator->second & METABOLITE_IS_LEFT)!=0)
                    compoundNamesIterator->second->linkedReactions.push_back(pair<Reaction*,unsigned int>
                        (&(reactionsIterator->second),METABOLITE_IS_LEFT));
                else
                    compoundNamesIterator->second->linkedReactions.push_back(pair<Reaction*,unsigned int>
                        (&(reactionsIterator->second),METABOLITE_IS_RIGHT));
            }
            ++equationMetabolitesIterator;
        }
        ++reactionsIterator;
    }
    linkedReactionsDirty = false;
}


void MyAlgorithms::calculateIsomeres(void)
{
}


void MyAlgorithms::findCompoundByFormula(const string& pattern)
{
    unsigned int numCompounds=0;
    pool->selectedCompounds->clear();
    
    T_compoundsIterator compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        if(compoundsIterator->second.formula.matches(pattern))
        {
            pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
            ++numCompounds;
        }
        compoundsIterator++;
    }

    if(numCompounds==1)
        emit addLogLine(QString().setNum(numCompounds) + " compound found");
    else
        emit addLogLine(QString().setNum(numCompounds) + " compounds found");
}

void MyAlgorithms::findCompounds(const QStringList& pattern, 
    std::set<Compound*>& foundCompounds, bool& unique, bool append)
{
    string metabolite;
    string temp2;
    pool->selectedCompounds->clear();

    if(!append)
        foundCompounds.clear();


    for(unsigned int n=0;n<pattern.size();n++)
    {
        pool->selectedCompounds->clear();
        metabolite = pattern[n].toAscii();
        if(metabolite.empty())
            continue;
        if(metabolite[0]=='$') // convert compound name to compound id
        {
            findCompoundByName(metabolite.substr(1,metabolite.size()-1));
        }
        else
        {
            findCompoundByName(metabolite);
            findCompoundById(metabolite);
        }

        if(pool->selectedCompounds->size()==1 || unique == false)
        {
            T_selectedCompoundsIterator selectedCompoundsIterator = pool->selectedCompounds->begin();
            while(selectedCompoundsIterator !=  pool->selectedCompounds->end())
            {
                //foundMetaboliteTypes->insert(pair<unsigned int, Compound*>
                //(foundMetaboliteTypes->size(),&(*(pool->compounds))[*selectedCompoundsIterator]));
                //foundCompounds.push_back(&(*(pool->compounds))[*selectedCompoundsIterator]);
                foundCompounds.insert(&(*(pool->compounds))[*selectedCompoundsIterator]);
                selectedCompoundsIterator++;
            }           
        }
        else if(pool->selectedCompounds->size()>0)
        {
            emit addLogLine("Compound specification has to be unique!");
            unique=false;
            return;
        }
        else
        {
            emit addLogLine(QString("Compound ") + metabolite.substr(1,metabolite.size()-1).c_str() + " not found");
            unique = false;
            return;
        }
    }
}

void MyAlgorithms::findCompoundById(const std::string& pattern)
{
    T_compoundsIterator compoundsIterator;
    unsigned int numCompounds = 0;

    compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        if(compoundsIterator->second.cNumber == pattern)
        {
            pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
            numCompounds++;
        }

        ++compoundsIterator;
    }      

    if(numCompounds==1)
        emit addLogLine(QString().setNum(numCompounds) + " compound found");
    else
        emit addLogLine(QString().setNum(numCompounds) + " compounds found");
}

void MyAlgorithms::findCompoundByName(const string& pattern)
{
    int val;
    int nameSize;
    unsigned int numCompounds=0;
    string pattern2;
    pattern2 = make_lowercase(pattern);
    val = pattern.find("*");
    T_compoundsIterator compoundsIterator;
    if(val == string::npos) // exact search
    {
        compoundsIterator = pool->compounds->begin();
        while(compoundsIterator != pool->compounds->end())
        {
            vector<string>::iterator namesIterator;
            namesIterator = compoundsIterator->second.names.begin();
            while(namesIterator != compoundsIterator->second.names.end())
            {
                if(make_lowercase(namesIterator->c_str())==pattern2)
                {
                    pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                    numCompounds++;
                }
                ++namesIterator;
            }
            /*
            nameSize = compoundsIterator->second.names.size();
            for(unsigned int i=0;i<nameSize;i++)
            {
                if(make_lowercase(compoundsIterator->second.names[i])==pattern2)
                {
                    pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                    numCompounds++;
                }
            }
            */
            compoundsIterator++;
        }
    }
    else
    {
        if(val==(pattern.size()-1)) // star at end
        {
            pattern2 = pattern2.substr(0,val);

            compoundsIterator = pool->compounds->begin();
            while(compoundsIterator != pool->compounds->end())
            {
                vector<string>::iterator namesIterator;
                namesIterator = compoundsIterator->second.names.begin();
                while(namesIterator != compoundsIterator->second.names.end())
                {
                    if(make_lowercase(namesIterator->c_str()).find(pattern2)==0)
                    {
                        pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                        numCompounds++;
                    }
                    ++namesIterator;
                }
                /*
                nameSize = compoundsIterator->second.names.size();
                for(unsigned int i=0;i<nameSize;i++)
                {
                    if(make_lowercase(compoundsIterator->second.names[i]).find(pattern2)==0)
                    {
                        pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                        numCompounds++;
                    }
                }
                */
                compoundsIterator++;
            }
        }
        else if(val==0) // start at beginning
        {
            pattern2 = pattern2.substr(1,pattern2.size()-1);

            compoundsIterator = pool->compounds->begin();
            while(compoundsIterator != pool->compounds->end())
            {
                vector<string>::iterator namesIterator;
                namesIterator = compoundsIterator->second.names.begin();
                while(namesIterator != compoundsIterator->second.names.end())
                {
                    if(make_lowercase(namesIterator->c_str()).find(pattern2)==0)
                    {
                        pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                        numCompounds++;
                    }
                    ++namesIterator;
                }
                /*
                nameSize = compoundsIterator->second.names.size();
                for(unsigned int i=0;i<nameSize;i++)
                {
                    if(make_lowercase(compoundsIterator->second.names[i]).find(pattern2)==0)
                    {
                        pool->selectedCompounds->insert(compoundsIterator->second.cNumber);
                        numCompounds++;
                    }
                }
                */
                compoundsIterator++;
            }
        }
        else
        {
            emit addLogLine("star in the middle not yet supported!");
            return;
        }
    }
    if(numCompounds==1)
        emit addLogLine(QString().setNum(numCompounds) + " compound found");
    else
        emit addLogLine(QString().setNum(numCompounds) + " compounds found");
}

void MyAlgorithms::findReplaceableMetabolites(void)
{
    unsigned int userDatasetSize = pool->userDatasets->size();
    unsigned int compoundsSize;
    map<string,string> synonymList;
    map<string,string> synonymNameList;
    map<string,unsigned int> nonMapped;
    set<string>::iterator sameAsIterator;
    T_compoundsIterator compoundsIterator;
    //map<string, unsigned int>::iterator equationCompoundsIterator;
    vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    vector<T_equationMetabolite>::iterator equationMetabolitesIterator;
    bool mapped;
    for(unsigned int n=0;n<userDatasetSize;n++)
    {
        equationMetabolitesIterator = (*(pool->userDatasets))[n].equation->metabolites.begin();
        while(equationMetabolitesIterator != (*(pool->userDatasets))[n].equation->metabolites.end())
        {
            if(equationMetabolitesIterator->first.compound->cNumber[0]!='C')
            {
                mapped=false;
                emit addLogLine(QString(string((*(pool->userDatasets))[n].scientist + " " 
                    + (*(pool->userDatasets))[n].keggID + ":"
                    +"metabolite " + equationMetabolitesIterator->first.compound->cNumber).c_str()));
                compoundsSize = (*(pool->compounds)).size();
                compoundsIterator = pool->compounds->begin();
                do
                {
                    sameAsIterator = compoundsIterator->second.sameAs.find(equationMetabolitesIterator->first.compound->cNumber);
                    if(sameAsIterator != compoundsIterator->second.sameAs.end())
                    {
                            mapped=true;
                            emit addLogLine(QString("-> can be replaced with ")+ compoundsIterator->second.cNumber.c_str());
                            if(synonymList.find(compoundsIterator->second.cNumber.c_str())==synonymList.end())
                            {
                                synonymList[compoundsIterator->second.cNumber.c_str()] = equationMetabolitesIterator->first.compound->cNumber;
                                synonymNameList[compoundsIterator->second.cNumber.c_str()] = compoundsIterator->second.names.begin()->c_str();
                            }
                    }
                    compoundsIterator++;
                }while(compoundsIterator != pool->compounds->end());
                if(mapped==false)
                {
                    nonMapped[equationMetabolitesIterator->first.compound->cNumber]=0;
                }
            }
            ++equationMetabolitesIterator;
        }

        /*
        equationCompoundsIterator = (*(pool->userDatasets))[n].equation->Compounds.begin();
        for(unsigned int i=0;i<(*(pool->userDatasets))[n].equation->Compounds.size();i++)
        {
            if(equationCompoundsIterator->first[0]!='C')
            {
                mapped=false;
                emit addLogLine(QString(string((*(pool->userDatasets))[n].scientist + " " 
                    + (*(pool->userDatasets))[n].keggID + ":"
                    +"metabolite " + equationCompoundsIterator->first).c_str()));
                compoundsSize = (*(pool->compounds)).size();
                compoundsIterator = pool->compounds->begin();
                do
                {
                    sameAsIterator = compoundsIterator->second.sameAs.find(equationCompoundsIterator->first);
                    if(sameAsIterator != compoundsIterator->second.sameAs.end())
                    {
                            mapped=true;
                            emit addLogLine(QString("-> can be replaced with ")+ compoundsIterator->second.cNumber.c_str());
                            if(synonymList.find(compoundsIterator->second.cNumber.c_str())==synonymList.end())
                            {
                                synonymList[compoundsIterator->second.cNumber.c_str()] = equationCompoundsIterator->first;
                                synonymNameList[compoundsIterator->second.cNumber.c_str()] = compoundsIterator->second.names[0];
                            }
                    }
                    compoundsIterator++;
                }while(compoundsIterator != pool->compounds->end());
                if(mapped==false)
                {
                    nonMapped[equationCompoundsIterator->first]=0;
                }
            }
            equationCompoundsIterator++;
        }

        */
    }
    emit addLogLine("\nSynonym-List:\n");
    map<string,string>::iterator synonymListIterator = synonymList.begin();
    for(unsigned int n=0;n<synonymList.size();n++)
    {
        emit addLogLine(string(synonymListIterator->first + " " + synonymListIterator->second + 
            " +" + synonymNameList[synonymListIterator->first]).c_str());
        synonymListIterator++;
    }
    string temp;
    emit addLogLine("\nThe following metabolites can not be mapped to C-Compounds:\n");
    map<string,unsigned int>::iterator nonMappedIterator= nonMapped.begin();
    for(unsigned int n=0;n<nonMapped.size();n++)
    {
        emit addLogLine(nonMappedIterator->first.c_str());
        nonMappedIterator++;
    }
}

MetaboliteObject* MyAlgorithms::getOrCreateMetaboliteObject(Compound *compound, 
    string compartment, const boost::numeric::ublas::vector<double>& label, bool create)
{
    map<string,MetaboliteObject*>::iterator metabolitesPoolIterator;
    MetaboliteObject *metaboliteObject;
    MetaboliteObject fake;
    fake.compound = compound;
    fake.compartment = compartment;
    fake.label = label;
    metabolitesPoolIterator = pool->metabolitesPool->find(fake.key());
    if(metabolitesPoolIterator == pool->metabolitesPool->end())
    {
        if(create == true)
        {
            metaboliteObject = new MetaboliteObject(compound,compartment);
            metaboliteObject->label = label;
            pool->metabolitesPool->insert(pair<string,MetaboliteObject*>(metaboliteObject->key(),metaboliteObject));
        }
        else
            metaboliteObject = 0;
    }
    else
        metaboliteObject = metabolitesPoolIterator->second;
    return metaboliteObject;
}

MetaboliteObject* MyAlgorithms::getOrCreateMetaboliteObject(Compound *compound, 
    string compartment, bool create)
{
    map<string,MetaboliteObject*>::iterator metabolitesPoolIterator;
    MetaboliteObject *metaboliteObject;
    metabolitesPoolIterator = pool->metabolitesPool->find(compound->cNumber+"_" +compartment);
    if(metabolitesPoolIterator == pool->metabolitesPool->end())
    {
        if(create == true)
        {
            metaboliteObject = new MetaboliteObject(compound,compartment);
            pool->metabolitesPool->insert(pair<string,MetaboliteObject*>(metaboliteObject->key(),metaboliteObject));
        }
        else
            metaboliteObject = 0;
    }
    else
        metaboliteObject = metabolitesPoolIterator->second;
    return metaboliteObject;
}

Compound* MyAlgorithms::getOrCreateCompound(string id)
{
    T_compoundsIterator compoundsIterator;
    compoundsIterator = pool->compounds->find(id);
    if(compoundsIterator == pool->compounds->end())
    {
        Compound tempCompound;
        compoundsIterator = pool->compounds->insert(pair<string, Compound>(id, tempCompound)).first;
        compoundsIterator->second.cNumber = id;
        return &(compoundsIterator->second);
    }
    else
        return &(compoundsIterator->second);
}

void MyAlgorithms::buildReactionSet(std::vector<Reaction*> *reactionSet,
                      bool includeUserReactions,
                      bool includeNotAccepted,
                      bool includeTransporter,
                      bool includeKegg)
{
    T_reactionsIterator reactionsIterator;
    //for(unsigned int n=0;n<pool->reactions->size();n++)
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.dataset != -1)
        {
            if(!includeUserReactions)
                continue;
            if(!(*pool->userDatasets)[reactionsIterator->second.dataset].accepted
                && !includeNotAccepted)
                    continue;  
            if(reactionsIterator->second.isTransporter &&
                !includeTransporter)    
                    continue;  
        }
        else if(!includeKegg)
            continue;
        reactionSet->push_back(&(reactionsIterator->second));
        ++reactionsIterator;
    }

}

void MyAlgorithms::findMetabolites(const QStringList& pattern, 
    std::vector<MetaboliteObject*>& foundMetabolites, bool append)
{
    set<Compound*> foundCompounds;
    set<Compound*>::iterator foundCompoundsIterator;
    bool unique;

    if(!append)
        foundMetabolites.clear();

    QStringList list;

    for(unsigned int n=0;n<pattern.size();n++)
    {

        list.clear();
        list << getCompoundFromMetabolite(pattern[n]);

        unique = true;
        findCompounds(
            list,
            foundCompounds,
            unique);
        
        if(!unique)
            return;

        MetaboliteObject* tempMetaboliteObject;
        foundCompoundsIterator = foundCompounds.begin();
        while(foundCompoundsIterator != foundCompounds.end())
        {
            tempMetaboliteObject = getOrCreateMetaboliteObject(*foundCompoundsIterator,
                string(getCompartmentFromMetabolite(pattern[n]).toLatin1()));
            if(tempMetaboliteObject != 0)
                foundMetabolites.push_back(tempMetaboliteObject);
            ++foundCompoundsIterator;
        }
    }

}

int MyAlgorithms::findReactions(vector<T_equationMetabolite>& equationMetabolites, 
    unsigned int eqnDirection, const set<Reaction*>& reactionSet, const bool& append)
{
    unsigned int numFound=0;

    if(!append)
        pool->selectedReactions2->clear();

    vector<T_equationMetabolite>::iterator equationMetabolitesIterator;
    set<Reaction*>::const_iterator reactionsIterator;
    bool found;

    reactionsIterator = reactionSet.begin();
    while(reactionsIterator != reactionSet.end())
    {
        found = true;
        equationMetabolitesIterator = equationMetabolites.begin();
        while(equationMetabolitesIterator != equationMetabolites.end())
        {
            if(!(*reactionsIterator)->equation->containsMetabolite(*equationMetabolitesIterator))
            {
                found = false;
                break;
            }
            ++equationMetabolitesIterator;
        }
        if(found)
        {
            //pool->selectedReactions2->push_back(*reactionsIterator);
            pool->selectedReactions2->insert(*reactionsIterator);
            numFound++;
        }
        ++reactionsIterator;
    }
    return numFound;
}

void twistLeftRightInEquationMetabolites(vector<T_equationMetabolite>& equationMetabolites)
{
    vector<T_equationMetabolite>::iterator equationMetabolitesIterator;
    equationMetabolitesIterator = equationMetabolites.begin();
    while(equationMetabolitesIterator != equationMetabolites.end())
    {
        if(equationMetabolitesIterator->second == METABOLITE_IS_LEFT)
            equationMetabolitesIterator->second = METABOLITE_IS_RIGHT;
        else
            equationMetabolitesIterator->second = METABOLITE_IS_LEFT;
        ++equationMetabolitesIterator;
    }
}

void MetabolitesToEquationMetabolites(vector<MetaboliteObject*> metabolites, 
    std::vector<T_equationMetabolite>& equationMetabolites, 
    const unsigned int& value, const bool& append)
{
    if(!append)
        equationMetabolites.clear();

    vector<MetaboliteObject*>::iterator metaboliteIterator;
    metaboliteIterator = metabolites.begin();
    while(metaboliteIterator != metabolites.end())
    {
        equationMetabolites.push_back(pair<MetaboliteObject, unsigned int>
            (**metaboliteIterator, value));
        ++metaboliteIterator;
    }
}

QString getCompoundFromMetabolite(const QString& pattern)
{
    unsigned int index = pattern.indexOf("_");
    if(index == -1)
        return pattern;
    return pattern.left(index);
}

QString getCompartmentFromMetabolite(const QString& pattern)
{
    unsigned int index = pattern.indexOf("_");
    if(index == -1)
        return "";
    return pattern.right(pattern.length() - index - 1);
}