#include "ImportRPairsFromKegg.h"
#include "CompoundDefinition.h"
#include "CompoundTransition.h"
#include "pool.h"
#include "MyAlgorithms.h"


#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>

using namespace std;
using namespace boost::spirit;


struct AddEntry
{
    AddEntry(ImportRPairsFromKegg& self) : self(self)
    {
    }
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        CompoundDefinition compoundDefinition1;
        CompoundDefinition compoundDefinition2;
        CompoundTransition compoundTransition;

        try
        {
            if(self.pool->compoundDefinitions.find(self.cur_compound1) == self.pool->compoundDefinitions.end())
            {
                compoundDefinition1.parseFromKeggRPair(self.cur_entry1);
                self.pool->compoundDefinitions.insert(pair<string,CompoundDefinition>
                    (self.cur_compound1, compoundDefinition1));
            }
            else
                compoundDefinition1 = (self.pool->compoundDefinitions.find(self.cur_compound1)->second);
            if(self.pool->compoundDefinitions.find(self.cur_compound2) == self.pool->compoundDefinitions.end())
            {
                compoundDefinition2.parseFromKeggRPair(self.cur_entry2);
                self.pool->compoundDefinitions.insert(pair<string,CompoundDefinition>
                    (self.cur_compound2, compoundDefinition2));
            }
            else
                compoundDefinition2 = (self.pool->compoundDefinitions.find(self.cur_compound2)->second);

            compoundTransition.setupMatrix(compoundDefinition1.length(), compoundDefinition2.length());
            
            compoundTransition.parseFromKeggRPair(self.cur_align);

            compoundTransition.compoundPairID.first = self.cur_compound1;
            compoundTransition.compoundPairID.second = self.cur_compound2;

            // need to add compoundPair

            self.calculateMatrix(compoundTransition);    
        }
        catch(std::exception &e)
        {
            // widges must be created in gui threads
            //QMessageBox::critical(0,QString("Error"),QString(e.what()));
            ++self.count;
            return;
        }    
        string temp;
        compoundTransition.generateRPairsID(temp);
        // some rpair id's occur twice, ie C00007_C00027
        /*
        if(self.pool->rpairs.find(temp) == self.pool->rpairs.end())
            self.pool->rpairs.insert(pair<string,CompoundTransition*>(temp, &compoundTransition));
        else
            int problem=10;
            */
        compoundTransition.inReactionsID = self.cur_reactionIDs;
        self.cur_reactionIDs.clear();
        //self.pool->compoundTransitions.push_back(compoundTransition);
        vector<string>::iterator inReactionsIterator = compoundTransition.inReactionsID.begin();
        while(inReactionsIterator != compoundTransition.inReactionsID.end())
        {
            self.pool->compoundTransitions.insert(pair<string,CompoundTransition>(*inReactionsIterator, compoundTransition));
            ++inReactionsIterator;
        }
        ++self.count;
    };
    ImportRPairsFromKegg& self;
};

ImportRPairsFromKegg::ImportRPairsFromKegg(std::string& text, 
    Pool *pool, 
    MyAlgorithms *myAlgorithms,
    unsigned int& count) : 
    pool(pool), myAlgorithms(myAlgorithms), count(count)
{
    AddEntry addEntry(*this);


    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;


    t_rule pairEntry = 
        str_p("ENTRY") 
        >> +blank_p
        >> ch_p('A')
        >> uint5_p
        >> +((anychar_p | blank_p) - eol_p)
        >> eol_p;
    t_rule name =
        str_p("NAME")
        >> +blank_p
        >> +(anychar_p - blank_p - eol_p)[assign_a(cur_name)]
        >> eol_p;

        t_rule compoundName = 
            +(anychar_p - eol_p)
            >>
            *(
                eol_p
                >> str_p("                    ")
                >> +(anychar_p - eol_p)
            );
    t_rule compound = 
        str_p("COMPOUND")
        >> +space_p
        >> (
            ch_p('C')
            >> uint5_p )[assign_a(cur_compound1)]
        >> +blank_p
        >> compoundName[assign_a(cur_compound1Name)]
        >> eol_p
        >> +space_p
        >> (
            ch_p('C')
            >> uint5_p)[assign_a(cur_compound2)]
        >> +blank_p
        >> compoundName[assign_a(cur_compound2Name)]
        >> eol_p;
        



    t_rule type = 
        str_p("TYPE")
        >> +blank_p
        >> *(anychar_p - eol_p)
        >> eol_p;
    t_rule rdm = 
        str_p("RDM")
        >> 
        *(
            +space_p
            >> int_p
            >> *space_p
            >> *(anychar_p - eol_p)
            >> eol_p
        );
    t_rule relatedPair = 
        str_p("RELATEDPAIR")
        >> 
        *(
            +space_p
            >> +(anychar_p - eol_p)
            >> eol_p
            );
    t_rule reaction = 
        str_p("REACTION")
        >>
        *(
            +blank_p>>+( 
                (ch_p('R') >> uint5_p)[push_back_a(cur_reactionIDs)] >> *blank_p
                )
            >> eol_p
        );
    t_rule align = 
        str_p("ALIGN")
        >> +space_p
        >> int_p
        >> eol_p
        >> *(+space_p >> *(anychar_p - eol_p) >> eol_p);
    t_rule compoundEntry = 
        str_p("ENTRY")
        >> int_p
        >> eol_p
        >> *(+space_p >> *(anychar_p - eol_p) >> eol_p);
    t_rule separationLine = 
        str_p("///")
        >> eol_p;

    t_rule main = 
        (*(
        pairEntry
        >> name
        >> compound
        >> *type
        >> *rdm
        >> *relatedPair
        >> *reaction
        >> align[assign_a(cur_align)]
        >> compoundEntry[assign_a(cur_entry1)]
        >> compoundEntry[assign_a(cur_entry2)]
        >> *separationLine
        )[addEntry]);


    BOOST_SPIRIT_DEBUG_NODE(main);
    BOOST_SPIRIT_DEBUG_NODE(pairEntry);
    BOOST_SPIRIT_DEBUG_NODE(name);
    BOOST_SPIRIT_DEBUG_NODE(compoundName);
    BOOST_SPIRIT_DEBUG_NODE(compound);
    BOOST_SPIRIT_DEBUG_NODE(type);
    BOOST_SPIRIT_DEBUG_NODE(rdm);
    BOOST_SPIRIT_DEBUG_NODE(relatedPair);
    BOOST_SPIRIT_DEBUG_NODE(reaction);
    BOOST_SPIRIT_DEBUG_NODE(align);
    BOOST_SPIRIT_DEBUG_NODE(compoundEntry);
    BOOST_SPIRIT_DEBUG_NODE(separationLine);


    if(!parse(text.c_str(), main).full)
    {
        throw std::exception("error parsing rpairs");
    }
   
}

void ImportRPairsFromKegg::calculateMatrix(CompoundTransition& compoundTransition)
{
    if(compoundTransition.base == "kegg")
    {
        T_rawInformationIterator rawInformationIterator= compoundTransition.rawInformation.begin();
        while(rawInformationIterator != compoundTransition.rawInformation.end())
        {
            if(rawInformationIterator->first > 0 && rawInformationIterator->second > 0) 
            {

                compoundTransition.transitionMatrix->operator()
                    (rawInformationIterator->first-1,rawInformationIterator->second-1)=1;
            }
            ++rawInformationIterator;
        }
    }
}



ImportRPairsFromKegg::~ImportRPairsFromKegg(void)
{
}
