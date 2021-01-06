#include "CompoundDefinition.h"
#include "spirit_include.h"

using namespace std;
using namespace boost::spirit;

CompoundDefinition::CompoundDefinition(void)
{
}

CompoundDefinition::~CompoundDefinition(void)
{
}

void CompoundDefinition::parseFromKeggRPair(string text)
{
    Atom a;
    atoms.clear();
    compoundID.clear();

    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;
    uint_parser<unsigned, 10, 5, 5> uint5_p;

    unsigned int numAtoms;

    t_rule ENTRY = 
        str_p("ENTRY")
        >> int_p
        >> eol_p;
    t_rule COMPOUND = 
        +blank_p
        >>str_p("COMPOUND")
        >>+blank_p
        >>(ch_p('C')
        >>uint5_p)[assign_a(compoundID)]
        >>eol_p;
    t_rule ATOM = 
        +blank_p
        >>str_p("ATOM")
        >>+blank_p
        >>int_p[assign_a(numAtoms)]
        >>eol_p;
    t_rule ATOMLINE =
        +blank_p
        >>int_p
        >>+blank_p
        >>+(anychar_p - blank_p - eol_p)[assign_a(a.symbol)]
        >>+blank_p
        >>+(anychar_p - blank_p - eol_p)[assign_a(a.atomLetter)]
        >>+blank_p
        >>real_p[assign_a(a.x)]
        >>+blank_p
        >>real_p[assign_a(a.y)]
        >>*(+blank_p>>*(anychar_p-eol_p))
        >>eol_p;

    t_rule BOND = 
        +blank_p
        >>str_p("BOND")
        >>+blank_p
        >>int_p
        >>eol_p;

    t_rule BONDLINE =
        +blank_p
        >>*((anychar_p | blank_p) - eol_p)
        >>eol_p;

    t_rule MAIN = 
        ENTRY
        >>COMPOUND
        >>ATOM
        >>+(ATOMLINE[push_back_a(atoms,a)])
        >>BOND
        >>*BONDLINE;

    BOOST_SPIRIT_DEBUG_NODE(ENTRY);
    BOOST_SPIRIT_DEBUG_NODE(COMPOUND);
    BOOST_SPIRIT_DEBUG_NODE(ATOM);
    BOOST_SPIRIT_DEBUG_NODE(ATOMLINE);
    BOOST_SPIRIT_DEBUG_NODE(BOND);
    BOOST_SPIRIT_DEBUG_NODE(BONDLINE);
    BOOST_SPIRIT_DEBUG_NODE(MAIN);

    if(!parse(text.c_str(), MAIN).full)
        throw std::exception("syntax error in compound definition!");

    if(atoms.size() != numAtoms)
        throw std::exception("some atoms are missing in description!");
}

unsigned int CompoundDefinition::length(void)
{
    return atoms.size();
}