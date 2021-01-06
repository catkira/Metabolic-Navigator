#include "ImportFateMapsMu.h"
#include "FateMapCompound.h"
#include "Pool.h"
#include "CompoundTransition.h"
#include "Compound.h"
#include "Reaction.h"
#include "Equation.h"
#include "Organism.h"

#include <QFile>
#include <QDir>

#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/insert_key_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

using namespace std;
using namespace boost::spirit;

namespace mu
{

class MappingInfo
{
public:
    std::string inchi;
    int localAtomNumber;
    int globalAtomNumber;
    bool equationSide;
};

struct EquationCompound
{
    std::string inchi;
    std::string mapping;
    bool equationSide;
};

class AddEntry
{
private:
    ImportFateMaps& self;


public:
    AddEntry(ImportFateMaps& self) : self(self)
    {
    }
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {

        static std::vector<MappingInfo> mapping;
        static std::vector<MappingInfo>::iterator mappingIterator,mappingIterator2;
        static std::map<std::string, Compound*>::iterator inchi2compoundIterator;
        static struct MappingInfo mappingInfo;

        typedef boost::spirit::rule<> t_rule;
        
        t_rule INCHI = str_p("InChI=") >> *(anychar_p - ch_p('"') - eol_p);

        t_rule MAPPING = ( ch_p('c') | ch_p('C') ) >> int_p[assign_a(mappingInfo.localAtomNumber)] >> ch_p('%') 
            >> int_p[assign_a(mappingInfo.globalAtomNumber)];

        t_rule MAPPING_ENTRY = ch_p('(') >> 
            MAPPING[push_back_a(mapping, mappingInfo)]
            >> *(ch_p(',') >> MAPPING[push_back_a(mapping, mappingInfo)])
            >> ch_p(')');

        t_rule COMPOUND = +ch_p('"') >> INCHI[assign_a(mappingInfo.inchi)] >> +ch_p('"') >> MAPPING_ENTRY;

        t_rule EQUATION = COMPOUND >> *(ch_p('+') >> COMPOUND)
            >> str_p(" <-> ")[assign_a(mappingInfo.equationSide,1)]  
            >> COMPOUND >> *(ch_p('+') >> COMPOUND) >> *ch_p('"');

        mappingInfo.equationSide = 0;
        mapping.clear();

        BOOST_SPIRIT_DEBUG_NODE(INCHI);
        BOOST_SPIRIT_DEBUG_NODE(MAPPING);
        BOOST_SPIRIT_DEBUG_NODE(MAPPING_ENTRY);
        BOOST_SPIRIT_DEBUG_NODE(COMPOUND);
        BOOST_SPIRIT_DEBUG_NODE(EQUATION);


        if(!parse(self.reactionMu.inchi.c_str(), EQUATION).full)
        {
            throw std::exception("error parsing equation");
        }

        // create interference list
        static map<string, CompoundTransition> transitions;
        static map<string, CompoundTransition>::iterator transitionsIterator;
        transitions.clear();
        CompoundTransition compoundTransition;
        static string temp;
        Reaction reaction;
        reaction.id = self.reactionMu.rnumber;
        reaction.source = "mu";
        multimap<Compound*,unsigned int> reactionCompounds;

        mappingIterator = mapping.begin();
        while(mappingIterator != mapping.end())
        {
            mappingIterator2 = mapping.begin();
            
            while(mappingIterator2 != mapping.end())
            {
                temp = mappingIterator2->inchi + mappingIterator->inchi + self.reactionMu.rnumber;

                if((mappingIterator2 != mappingIterator) && transitions.find(temp)==transitions.end())                         
                    // check if reverse compound transition already exists
                {
                    if(mappingIterator2->globalAtomNumber == mappingIterator->globalAtomNumber)
                    {

                        temp = mappingIterator->inchi + mappingIterator2->inchi + self.reactionMu.rnumber;

                        transitionsIterator = transitions.find(temp);
                        if(transitionsIterator == transitions.end())
                        {
                            compoundTransition.rawInformation.clear();
                            compoundTransition.inReactionsID.clear();
                            compoundTransition.inReactionsID.push_back(self.reactionMu.rnumber);


                            Compound *compound1,*compound2;

                            inchi2compoundIterator = self.pool->inchi2Compound.find(mappingIterator->inchi);
                            if(inchi2compoundIterator != self.pool->inchi2Compound.end())
                                compound1 = inchi2compoundIterator->second;
                            else
                                throw std::exception("unknown inchi string");
                            inchi2compoundIterator = self.pool->inchi2Compound.find(mappingIterator2->inchi);
                            if(inchi2compoundIterator != self.pool->inchi2Compound.end())
                                compound2 = inchi2compoundIterator->second;
                            else
                                throw std::exception("unknown inchi string");

                            compoundTransition.compoundPair.first = compound1;
                            compoundTransition.compoundPair.second = compound2;
                            compoundTransition.compoundPairID.first = compound1->cNumber;
                            compoundTransition.compoundPairID.second = compound2->cNumber;

                            reactionCompounds.insert(pair<Compound*,unsigned int>(compound1, METABOLITE_IS_LEFT));
                            reactionCompounds.insert(pair<Compound*,unsigned int>(compound2, METABOLITE_IS_RIGHT));
                            
                            compoundTransition.rawInformation.push_back(pair<int,int>(mappingIterator->localAtomNumber,mappingIterator2->localAtomNumber));
                            transitions.insert(pair<string, CompoundTransition>(temp,compoundTransition));
                        }
                        else
                        {
                            // add atom transition to compoundTransition
                            transitionsIterator->second.rawInformation.push_back(pair<int,int>(mappingIterator->localAtomNumber,mappingIterator2->localAtomNumber));
                        }
                    }
                }
                ++mappingIterator2;
            }
            
            ++mappingIterator;
        }

        // add transitions and reaction to pool

        transitionsIterator = transitions.begin();
        while(transitionsIterator != transitions.end())
        {
            transitionsIterator->second.setupMatrix();
            transitionsIterator->second.calculateMatrix();
            if(!transitionsIterator->second.rawInformation.empty())
                transitionsIterator->second.type = "carb";

            self.pool->compoundTransitions.insert(pair<string, CompoundTransition>(self.reactionMu.rnumber, transitionsIterator->second));
            reaction.linkedCompoundTransitions.push_back(&(transitionsIterator->second));

            ++transitionsIterator;
        }

        /*
        reaction.equation->parseEquation(self.myAlgorithms,reactionCompounds,self.pool->metabolitesPool);
        reaction.names.push_back(self.reactionMu.name);
        self.pool->reactions->insert(pair<string,Reaction>(reaction.id, reaction));
        */

    };

};

ImportFateMaps::ImportFateMaps(Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count) : count(count), pool(pool), myAlgorithms(myAlgorithms)
{
}


ImportFateMaps::~ImportFateMaps(void)
{
}

void ImportFateMaps::readFiles(const string& dir)
{
    string reactions;
    string compounds;


    QFile file;
    //QDir::setCurrent(dir.c_str());
    QDir::setCurrent(QString(pool->dataDir.c_str()));

    file.setFileName("reactions.txt");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        throw std::exception("could not open reactions.txt");
    }
    reactions = file.readAll().constData();
    file.close();

    file.setFileName("compounds.txt");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        throw std::exception("could not open compounds.txt");
    }
    compounds = file.readAll().constData();
    file.close();

    readFiles(compounds, reactions);

    myAlgorithms->setLinkedReactionsDirty();
}



void ImportFateMaps::readFiles(const std::string& compounds, const std::string& reactions)
{

    readCompounds(compounds);

    // reactions
    readReactions(reactions);

    //emit requestUpdate(UPDATE_ORGANISMS);
}

void ImportFateMaps::readCompounds(const std::string& compounds)
{
    FateMapCompound fateMapCompound;

    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    // compounds

    count = 0;

    pool->fateMapCompounds.clear();

    vector<int> homotop;
    vector<int> prochiral;

    t_rule LINE_ENTRY = *(anychar_p - eol_p - ch_p(0x09));
    t_rule HOMOTOP = 
        ch_p('(')[clear_a(homotop)] >> 
        int_p[push_back_a(homotop)] >> +(ch_p(',') >> int_p[push_back_a(homotop)]) >>
        ch_p(')');
    t_rule PROCHIRAL = ch_p('(')[clear_a(prochiral)] >>
        int_p[push_back_a(prochiral)] >> +(ch_p(',') >> int_p[push_back_a(prochiral)]) >>
        ch_p(')');

    t_rule COMPOUND_HEADER = str_p("InChI") >> blank_p >> *(anychar_p - eol_p) >> eol_p;
    t_rule COMPOUND_NAME = str_p("; ") >>
                (*(anychar_p - eol_p - ch_p(';') - ch_p(0x09) - ch_p('"')))[push_back_a(fateMapCompound.names)] >> *ch_p('"');
    t_rule COMPOUND_LINE = 
        LINE_ENTRY[assign_a(fateMapCompound.inchi)] >> ch_p(0x09) >>
        LINE_ENTRY[assign_a(fateMapCompound.inchiAux)] >> ch_p(0x09) >>
        *ch_p('"') >> str_p("KEGG:")[clear_a(fateMapCompound.names)] >> 
            (ch_p('C') >> uint5_p)[assign_a(fateMapCompound.cnumber)] >> 
            *COMPOUND_NAME >>
        (ch_p(0x09)[clear_a(fateMapCompound.homotopes)])[clear_a(fateMapCompound.prochirales)]>>
        *ch_p('"') >>
            * ( HOMOTOP[push_back_a(fateMapCompound.homotopes, homotop)] >> *ch_p(';')) >> *ch_p('"')  >>
        ch_p(0x09) >> *ch_p('"') >> * ( PROCHIRAL[push_back_a(fateMapCompound.prochirales, prochiral)] >> *ch_p(';')) >> *ch_p('"')  >>
        ch_p(0x09) >> LINE_ENTRY >> 
        ch_p(0x09) >> LINE_ENTRY >> 
        *(ch_p(0x09) >> LINE_ENTRY) >> 
        *(ch_p(0x09) >> LINE_ENTRY) >> 
        eol_p[increment_a(count)];

    t_rule COMPOUND = *COMPOUND_HEADER >> * (COMPOUND_LINE[insert_at_a(pool->fateMapCompounds, fateMapCompound.cnumber, fateMapCompound)]);

    BOOST_SPIRIT_DEBUG_NODE(LINE_ENTRY);
    BOOST_SPIRIT_DEBUG_NODE(COMPOUND_HEADER);
    BOOST_SPIRIT_DEBUG_NODE(COMPOUND_NAME);
    BOOST_SPIRIT_DEBUG_NODE(COMPOUND_LINE);
    BOOST_SPIRIT_DEBUG_NODE(COMPOUND);


    if(!parse(compounds.c_str(), COMPOUND).full)
    {
        throw std::exception("error parsing compounds.txt");
    }
    
    // create inchi2compoundIDs and add fateMapCompounds to compounds

    std::map<std::string, FateMapCompound>::iterator fateMapCompoundsIterator;

    string tempString;
    Compound compound;
    Compound *compoundPointer;
    fateMapCompoundsIterator = pool->fateMapCompounds.begin();
    vector<vector<int>>::iterator homoIterator,proIterator;
    while(fateMapCompoundsIterator != pool->fateMapCompounds.end())
    {

        compoundPointer = myAlgorithms->getOrCreateCompound(fateMapCompoundsIterator->second.cnumber);
        //compound.cNumber = fateMapCompoundsIterator->second.cnumber;
        //compoundsIterator = pool->compounds->insert(pair<string,Compound>(compound.cNumber,compound)).first;
        
        pool->compoundNames->insert(pair<string,Compound*>(compoundPointer->cNumber, compoundPointer));

        fateMapCompoundsIterator->second.compound = compoundPointer;

        pool->inchi2Compound.insert(pair<string,Compound*>(fateMapCompoundsIterator->second.inchi, compoundPointer));

        compoundPointer->names = fateMapCompoundsIterator->second.names;


        parse(fateMapCompoundsIterator->second.inchi.c_str(), 
            str_p("InChI=1/") >>
            (*(anychar_p - ch_p('/')))[assign_a(tempString)] >> 
            ch_p('/')
            ); 
        compoundPointer->formula.parseFormula(tempString);
        compoundPointer->inchi = fateMapCompoundsIterator->second.inchi;

        compoundPointer->homotopes = fateMapCompoundsIterator->second.homotopes;
        compoundPointer->prochirales = fateMapCompoundsIterator->second.prochirales;

        /*
        erase homotopes which are prochirales
        */

        homoIterator = compoundPointer->homotopes.begin();
        bool homoBreak=false;
        while(homoIterator != compoundPointer->homotopes.end())
        {
            homoBreak=false;
            proIterator = compoundPointer->prochirales.begin();
            while(proIterator != compoundPointer->prochirales.end())
            {
                if(*proIterator == *homoIterator)
                {
                    homoIterator = compoundPointer->homotopes.erase(homoIterator);
                    homoBreak = true;
                    break;
                }
                ++proIterator;
            }
            if(!homoBreak)
                ++homoIterator;
        }
        fateMapCompoundsIterator->second.numberOfCarbons = compoundPointer->formula.getNumberOfCarbons();

        ++fateMapCompoundsIterator;
    }
}


void ImportFateMaps::readReactions(const std::string& reactions)
{

    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    pool->compoundTransitions.clear();



    class AddEntry addEntry(*this);

    t_rule LINE_ENTRY = *(anychar_p - eol_p - ch_p(0x09));

    t_rule REACTION_HEADER = str_p("Reaction") >> blank_p >> *(anychar_p - eol_p) >> eol_p;
    t_rule REACTION_LINE = 
        LINE_ENTRY[assign_a(reactionMu.name)] >>
        ch_p(0x09) >> LINE_ENTRY[assign_a(reactionMu.ecnumber)] >> 
        ch_p(0x09) >> LINE_ENTRY[assign_a(reactionMu.inchi)] >> 
        ch_p(0x09) >> LINE_ENTRY >> 
        ch_p(0x09) >> LINE_ENTRY[assign_a(reactionMu.rnumber)] >> 
        ch_p(0x09) >> LINE_ENTRY[assign_a(reactionMu.link)] >> 
        *ch_p(0x09) >> eol_p[increment_a(count)];

    t_rule REACTION = *REACTION_HEADER >> * ((ch_p(';') >> *(anychar_p - eol_p) >> eol_p) | REACTION_LINE[addEntry]);

    if(!parse(reactions.c_str(), REACTION).full)
    {
        throw std::exception("error parsing reactions.txt");
    }

    /*

    Organism organism;
    organism.setName("Kegg_Mu");


    T_reactionsIterator reactionsIterator = pool->reactions->begin();
    while(reactionsIterator != pool->reactions->end())
    {
        if(reactionsIterator->second.source == "mu")
        {
            organism.reactions.insert(&(reactionsIterator->second));
        }
        ++reactionsIterator;
    }


    pool->organisms.insert(pair<string, Organism>(organism.name(),organism));
    */
}


}