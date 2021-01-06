#include "ImportEnzymesFromKegg.h"
#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/insert_key_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <map>
#include <cctype>

#include "Pool.h"
#include "Enzyme.h"
#include "MyAlgorithms.h"
#include "Organism.h"
#include "Reaction.h"

using namespace std;
using namespace boost::spirit;


ImportEnzymesFromKegg::ImportEnzymesFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count)
    : pool(pool), myAlgorithms(myAlgorithms), count(count)
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;
    uint_parser<unsigned, 10, 5, 5> uint5_p;

    Enzyme tempEnzyme;

    t_rule NAME = +space_p >> (*(anychar_p - ch_p(';') - eol_p))[push_back_a(tempEnzyme.names)] >> *ch_p(';') >> eol_p;
    t_rule NAMES = str_p("NAME") >> +NAME;

    t_rule REACTION = (ch_p('R')>>uint5_p)[push_back_a(tempEnzyme.inReactions)];

    t_rule TOLINEEND = *(anychar_p - eol_p) >> eol_p;

    string temp;

    t_rule GENE = +space_p >> 
        (
        (
            (range_p('A','Z') >> range_p('A','Z') >> range_p('A','Z'))[assign_a(temp)] >>
            ch_p(':') >> TOLINEEND
        )[push_back_a(tempEnzyme.inOrganisms,temp)]
        |
        (
            *(anychar_p - eol_p) >> eol_p
        ));
    t_rule GENES = str_p("GENES") >> +GENE;

    t_rule REACTIONS = str_p("ALL_REAC") >> 
        *(space_p| str_p("(other)")) >> 
        +(REACTION >> *(str_p(" > ") | space_p | (ch_p(';') >> eol_p) | str_p("(other)")));


    t_rule ENTRY = ((str_p("ENTRY")[clear_a(tempEnzyme.names)])[clear_a(tempEnzyme.inReactions)])[clear_a(tempEnzyme.inOrganisms)] >> *space_p >>
        str_p("EC ") >> *(anychar_p - space_p) >> *(anychar_p - eol_p) >> eol_p >>
        *NAMES >> 
        *((*(anychar_p - eol_p) >> eol_p) - ((str_p("ALL_REAC")| str_p("ENTRY")) >> *(anychar_p - eol_p) >> eol_p)) >>
        *REACTIONS >> 
        *((*(anychar_p - eol_p) >> eol_p) - ((str_p("GENES") | str_p("ENTRY")) >> *(anychar_p - eol_p) >> eol_p)) >>
        *GENES >>
        *((*(anychar_p - eol_p) >> eol_p) - ((str_p("ENTRY")) >> *(anychar_p - eol_p) >> eol_p));


    BOOST_SPIRIT_DEBUG_NODE(TOLINEEND);
    BOOST_SPIRIT_DEBUG_NODE(NAMES);
    BOOST_SPIRIT_DEBUG_NODE(NAME);
    BOOST_SPIRIT_DEBUG_NODE(GENE);
    BOOST_SPIRIT_DEBUG_NODE(GENES);
    BOOST_SPIRIT_DEBUG_NODE(REACTIONS);
    BOOST_SPIRIT_DEBUG_NODE(REACTION);
    BOOST_SPIRIT_DEBUG_NODE(ENTRY);

    pool->enzymes->clear();

    parse(text.c_str(), *( (ENTRY[increment_a(count)])[push_back_a(*pool->enzymes, tempEnzyme)] ));

}

void ImportEnzymesFromKegg::parseOrganismNames(const std::string& text)
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    string shortname,longname;

    t_rule LONGNAME = str_p("  <td align=\"left\"><a href=\"http://www.genome.jp/dbget-bin/www_bfind?")
        >> *(anychar_p - ch_p('>')) >> ch_p('>')
        >> (*(anychar_p - ch_p('<')))[assign_a(longname)] >> ch_p('<')
        >> *(anychar_p - eol_p) >> eol_p;
    t_rule SHORTNAME = str_p("  <td align=\"center\"><a href=\"http://www.genome.jp/kegg-bin/show_organism?")
        >> *(anychar_p - ch_p('>')) >> ch_p('>')
        >> (*(anychar_p - ch_p('<')))[assign_a(shortname)] >> ch_p('<');

    t_rule ANYLINE = *(anychar_p - eol_p) >> eol_p;

    t_rule ORGANISM = *(ANYLINE - LONGNAME) >> LONGNAME >> SHORTNAME;

    t_rule MAIN = *ORGANISM[insert_at_a(pool->organismNames,shortname,longname)];

    pool->organismNames.clear();


    parse(text.c_str(), MAIN);

}


ImportEnzymesFromKegg::~ImportEnzymesFromKegg(void)
{
}

void ImportEnzymesFromKegg::buildOrganisms(void)
{
    map<string, vector<string>> organismReactions;
    map<string, vector<string>>::iterator organismReactionsIterator;

    vector<string> tempVector;
    T_enzymesIterator enzymesIterator = pool->enzymes->begin();
    while(enzymesIterator != pool->enzymes->end())
    {
        T_inOrganismsIterator inOrganismsIterator = enzymesIterator->inOrganisms.begin();
        while(inOrganismsIterator != enzymesIterator->inOrganisms.end())
        {
            organismReactionsIterator = organismReactions.find(*inOrganismsIterator);
            if(organismReactionsIterator == organismReactions.end())
            {
                organismReactions.insert(pair<string, vector<string>>(*inOrganismsIterator,tempVector));
                organismReactionsIterator = organismReactions.find(*inOrganismsIterator);
            }
    
            T_inReactionsIterator inReactionsIterator = enzymesIterator->inReactions.begin();
            while(inReactionsIterator != enzymesIterator->inReactions.end())
            {
                organismReactionsIterator->second.push_back(*inReactionsIterator);
                ++inReactionsIterator;
            }

            ++inOrganismsIterator;
        }
        ++enzymesIterator;
    }

    vector<string>::iterator inReactionsIDIterator;
    map<string,Reaction>::iterator reactionsIterator;

    string lowercaseName;
    Organism tempOrganism;
    organismReactionsIterator = organismReactions.begin();
    while(organismReactionsIterator != organismReactions.end())
    {
        tempOrganism.reactionIDs.clear();
        tempOrganism.reactions.clear();

        lowercaseName = organismReactionsIterator->first;
        
        transform(lowercaseName.begin(),
            lowercaseName.end(),
            lowercaseName.begin(),
            tolower);
        if(pool->organismNames.find(lowercaseName) == pool->organismNames.end())
            tempOrganism.setName(organismReactionsIterator->first);
        else
        {
            tempOrganism.setName(organismReactionsIterator->first + " : " +
                pool->organismNames[lowercaseName]);
        
        }
       
        tempOrganism.reactionIDs = organismReactionsIterator->second;
        
        inReactionsIDIterator = tempOrganism.reactionIDs.begin();
        while(inReactionsIDIterator != tempOrganism.reactionIDs.end())
        {
            reactionsIterator = pool->reactions->find(*inReactionsIDIterator);
            if(reactionsIterator != pool->reactions->end())
            {
                tempOrganism.reactions.insert(&reactionsIterator->second);
            }
            ++inReactionsIDIterator;            
        }

        pool->organisms.insert(pair<string, Organism>(tempOrganism.name(), tempOrganism));
        ++organismReactionsIterator;
    }


}