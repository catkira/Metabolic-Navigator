#include "ImportOrganism.h"
#include "Organism.h"
#include "Pool.h"
#include "Reaction.h"

#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/insert_key_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

using namespace std;
using namespace boost::spirit;

ImportOrganism::ImportOrganism(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count)
    : pool(pool), myAlgorithms(myAlgorithms), count(count)
{

    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    Organism organism;
    string organismName;

    t_rule NAME = str_p("NAME:") 
        >> *space_p
        >> (*(anychar_p - eol_p))[assign_a(organismName)]
        >> eol_p;

    t_rule REACTION = 
        (+(anychar_p - space_p - eol_p))[push_back_a(organism.reactionIDs)]
        >> *space_p;

    t_rule REACTIONS = 
        str_p("REACTIONS:")
        >> +space_p
        >> *REACTION;

    t_rule MAIN = 
        NAME
        >> REACTIONS;

    BOOST_SPIRIT_DEBUG_NODE(NAME);
    BOOST_SPIRIT_DEBUG_NODE(REACTION);
    BOOST_SPIRIT_DEBUG_NODE(REACTIONS);
    BOOST_SPIRIT_DEBUG_NODE(MAIN);

    if(!parse(text.c_str(), MAIN).full)
    {
        throw std::exception("could not parse organism!");
    }

    vector<string>::iterator it;
    map<string, Reaction>::iterator reactionsIterator;
    it=organism.reactionIDs.begin();
    while(it != organism.reactionIDs.end())
    {
        reactionsIterator = pool->reactions->find(*it);
        if(reactionsIterator != pool->reactions->end())
        {
            organism.reactions.insert(&reactionsIterator->second);
        }
        ++it;
    }

    organism.setName(organismName);
    pool->organisms.insert(pair<string, Organism>(organismName, organism));

}

ImportOrganism::~ImportOrganism(void)
{
}
