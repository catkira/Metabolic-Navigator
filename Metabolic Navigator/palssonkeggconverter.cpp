#include "palssonkeggconverter.h"
#include "Pool.h"
#include <QFileDialog>
#include "Pool.h"
#include "Reaction.h"
#include "Compound.h"
#include "MyAlgorithms.h"
#include "Equation.h"
#include <QMessageBox>
#include <vector>

#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

using namespace std;
using namespace boost::spirit;

class AddPalssonReaction
{
private:
    PalssonKeggConverter& self;


public:
    AddPalssonReaction(PalssonKeggConverter& self) : self(self)
    {
    }
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        Reaction reaction;
        reaction.id = self.reactionPalsson.id;

        multimap<string,string>::iterator dictIt;
        std::multimap<Compound*, unsigned int> equation;
        Compound *tempCompound;

        vector<string>::iterator it;
        it = self.reactionPalsson.leftCompounds.begin();
        while(it != self.reactionPalsson.leftCompounds.end())
        {
            dictIt = self.palssonKeggCompounds.find(*it);
            if(dictIt == self.palssonKeggCompounds.end())
            {
                //throw std::exception("compound not found in dictionary!");
                ++it;
                continue;
            }
            tempCompound = self.myAlgorithms->getOrCreateCompound(dictIt->second);
            equation.insert(pair<Compound*,unsigned int>(tempCompound,METABOLITE_IS_LEFT ));
            ++it;
        }

        it = self.reactionPalsson.rightCompounds.begin();
        while(it != self.reactionPalsson.rightCompounds.end())
        {
            dictIt = self.palssonKeggCompounds.find(*it);
            if(dictIt == self.palssonKeggCompounds.end())
            {
                //throw std::exception("compound not found in dictionary!");
                ++it;
                continue;
            }
            tempCompound = self.myAlgorithms->getOrCreateCompound(dictIt->second);
            equation.insert(pair<Compound*,unsigned int>(tempCompound,METABOLITE_IS_RIGHT));
            ++it;
        }
        
        reaction.equation->parseEquation(self.myAlgorithms, equation, 0);
        reaction.source = "palsson";
        reaction.equation->directionType = self.reactionPalsson.directionType;

        self.pool->reactions->insert(pair<string,Reaction>(reaction.id, reaction));
    }
};

PalssonKeggConverter::PalssonKeggConverter(QWidget *parent, Pool *pool, MyAlgorithms *myAlgorithms)
    : QDialog(parent), pool(pool), myAlgorithms(myAlgorithms)
{
    ui.setupUi(this);
}

PalssonKeggConverter::~PalssonKeggConverter()
{

}

void PalssonKeggConverter::updateTable(void)
{
}

void PalssonKeggConverter::on_pushButton_3_clicked()
{
    close();
}

void PalssonKeggConverter::on_pushButton_clicked()
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    palssonKeggCompounds.clear();
    keggPalssonCompounds.clear();

    QString fileName = QFileDialog::getOpenFileName();
    if(fileName != "")
    {
        QFile file;
        QDir::setCurrent(fileName);

        file.setFileName(fileName);
        if(file.open(QIODevice::ReadOnly)!=true)
        {
            ui.lineEdit_2->setText("error");
            ui.lineEdit_3->setText("0");
            return;
        }
        ui.lineEdit->setText(fileName);
        string bytes = file.readAll().constData();
        file.close();

        string keggCompound, pCompound;

        t_rule KEGG_COMPOUND = (ch_p('C') | ch_p('G')) >> uint5_p;
        
        t_rule P_COMPOUND = *(anychar_p - eol_p - space_p);

        //t_rule P_COMPOUND = *(anychar_p - eol_p - space_p)
            //>> *(space_p >> (*(anychar_p - eol_p - space_p) - KEGG_COMPOUND));

        t_rule K_P_LINE =  KEGG_COMPOUND[assign_a(keggCompound)] 
            >> +space_p 
            >> P_COMPOUND[assign_a(pCompound)]
            >> eol_p;
        t_rule P_K_LINE = P_COMPOUND[assign_a(pCompound)]
            >> +space_p 
            >> KEGG_COMPOUND[assign_a(keggCompound)]  
            >> eol_p;
        t_rule LINE = ((K_P_LINE
            ^ P_K_LINE)[insert_at_a(keggPalssonCompounds, keggCompound, pCompound)])[insert_at_a(palssonKeggCompounds, pCompound, keggCompound)];



        BOOST_SPIRIT_DEBUG_NODE(KEGG_COMPOUND);
        BOOST_SPIRIT_DEBUG_NODE(P_COMPOUND);
        BOOST_SPIRIT_DEBUG_NODE(K_P_LINE);
        BOOST_SPIRIT_DEBUG_NODE(P_K_LINE);
        BOOST_SPIRIT_DEBUG_NODE(LINE);

        if(parse(bytes.c_str(), *LINE).full)
        {
            ui.lineEdit_2->setText("ok");
        }
        else
            ui.lineEdit_2->setText("error");

        ui.lineEdit_3->setText(QString().setNum(palssonKeggCompounds.size()));
    }
}

void PalssonKeggConverter::on_pushButton_2_clicked()
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    vector<Reaction*> reactionsT;
    vector<Reaction*>::iterator reactionsTIterator;

    string tempText;


    if(pool->compounds->size() == 0)
    {
        QMessageBox::critical(0, "Problem","You need to import kegg compounds first",QMessageBox::Ok);
        return;
    }

    if(pool->reactions->size() == 0)
    {
        QMessageBox::critical(0, "Problem","You need to import kegg reactions first",QMessageBox::Ok);
        return;
    }

    QString fileName = QFileDialog::getOpenFileName();
    if(fileName != "")
    {
        QFile file;
        QDir::setCurrent(fileName);

        file.setFileName(fileName);
        if(file.open(QIODevice::ReadOnly)!=true)
        {
            return;
        }

        ui.lineEdit_4->setText(fileName);

        string bytes = file.readAll().constData();
        file.close();

        AddPalssonReaction addPalssonReaction(*this);

        t_rule REACTION_ID = *(anychar_p - eol_p - space_p);
        t_rule COMPOUND_LEFT = *(real_p >> +space_p) >> (+(anychar_p - eol_p - space_p))[push_back_a(reactionPalsson.leftCompounds)];
        t_rule COMPOUND_RIGHT = *(real_p >> +space_p) >> (+(anychar_p - eol_p - space_p))[push_back_a(reactionPalsson.rightCompounds)];
        t_rule COMPOUNDS_LEFT = COMPOUND_LEFT 
            >> *( *space_p >> ch_p('+') >> *space_p >> COMPOUND_LEFT);
        t_rule COMPOUNDS_RIGHT = COMPOUND_RIGHT
            >> *( *space_p >> ch_p('+') >> *space_p >> COMPOUND_RIGHT );
        t_rule REACTION = 
            ((REACTION_ID[assign_a(reactionPalsson.id)])[clear_a(reactionPalsson.leftCompounds)])[clear_a(reactionPalsson.rightCompounds)]
            >> *space_p
            >> COMPOUNDS_LEFT
            >> *space_p
            >> (str_p("<==>")[assign_a(reactionPalsson.directionType, direction_both)] 
                | str_p("-->") [assign_a(reactionPalsson.directionType, direction_right)] 
                | str_p("<--")[assign_a(reactionPalsson.directionType, direction_left)] )
            >> *space_p
            >> COMPOUNDS_RIGHT
            >> eol_p;

        BOOST_SPIRIT_DEBUG_NODE(REACTION_ID);
        BOOST_SPIRIT_DEBUG_NODE(COMPOUND_LEFT);
        BOOST_SPIRIT_DEBUG_NODE(COMPOUND_RIGHT);
        BOOST_SPIRIT_DEBUG_NODE(COMPOUNDS_LEFT);
        BOOST_SPIRIT_DEBUG_NODE(COMPOUNDS_RIGHT);
        BOOST_SPIRIT_DEBUG_NODE(REACTION);

        parse(bytes.c_str(), *(REACTION[addPalssonReaction]));

        ui.textEdit->clear();

        palssonKeggMapping.clear();
        T_reactionsIterator it,it2;
        bool isReverse;
        bool found = false;
        unsigned int numMappedReactions1=0;
        unsigned int numMappedReactions2=0;
        it = pool->reactions->begin();
        while(it != pool->reactions->end())
        {
            if(it->second.source != "palsson")
            {
                ++it;
                continue;
            }
            
            found = false;
            it2 = pool->reactions->begin();
            while(it2 != pool->reactions->end())
            {
                if(it == it2 || it2->second.source == "palsson")
                {
                    ++it2;
                    continue;
                }
                if(compareReactions(it->second, it2->second, isReverse))
                {
                    reactionsT.push_back(&it2->second);
                    if(isReverse)
                    {
                        tempText += it2->second.id + " " + it->second.id + " reverse\n";
                        palssonKeggMapping.insert(pair<string,pair<string,int>>
                            (it->second.id,pair<string,int>(it2->second.id,-1)));
                    }
                    else
                    {
                        tempText += it2->second.id + " " + it->second.id + "\n";
                        palssonKeggMapping.insert(pair<string,pair<string,int>>
                            (it->second.id,pair<string,int>(it2->second.id,1)));
                    }
                    found = true;
                    ++numMappedReactions1;
                    break;
                }
                ++it2;
            }

            if(found)
            {
                ++it;
                continue;
            }

            it2 = pool->reactions->begin();
            while(it2 != pool->reactions->end())
            {
                if(it == it2 || it2->second.source == "palsson")
                {
                    ++it2;
                    continue;
                }
                if(compareReactions2(it->second, it2->second, isReverse))
                {
                    reactionsT.push_back(&it2->second);
                    if(isReverse)
                    {
                        tempText += it2->second.id + " " + it->second.id + " reverse *\n";
                        palssonKeggMapping.insert(pair<string,pair<string,int>>
                            (it->second.id,pair<string,int>(it2->second.id,-1)));
                    }
                    else
                    {
                        tempText += it2->second.id + " " + it->second.id + " *\n";
                        palssonKeggMapping.insert(pair<string,pair<string,int>>
                            (it->second.id,pair<string,int>(it2->second.id,1)));
                    }
                    ++numMappedReactions2;
                    break;
                }
                ++it2;
            }


            ++it;
        }

        tempText += string(QString().setNum(numMappedReactions1).toAscii()) + 
            " reactions mapped exactly\n";
        tempText += string(QString().setNum(numMappedReactions2).toAscii()) + 
            " reactions mapped without protons\n";

        tempText +="\nNon mapped reactions\n";

        unsigned int numNonMapped=0;
        it = pool->reactions->begin();
        while(it != pool->reactions->end())
        {
            if(it->second.source != "palsson")
            {
                ++it;
                continue;
            }
            if(palssonKeggMapping.find(it->second.id) == palssonKeggMapping.end())
            {
                tempText += it->second.id + "\n";
                ++numNonMapped;
            }
            ++it;
        }
        tempText += "\n" + string(QString().setNum(numNonMapped).toAscii()) + " non mapped reactions";


        tempText += "\n\n -- ORGANISM -- \nNAME: palsson\nREACTIONS:\n";

        reactionsTIterator = reactionsT.begin();
        while(reactionsTIterator != reactionsT.end())
        {
            tempText += (*reactionsTIterator)->id + "\n";
            ++reactionsTIterator;
        }


        if(!fluxData.empty())
        {

            tempText +="\n\n -- Reaction user settings from FBA data --\n";

            map<string,pair<string,int>>::iterator mappingIterator;
            map<string,double>::iterator fluxDataIterator;
            mappingIterator = palssonKeggMapping.begin();
            int numMapped = 0;
            while(mappingIterator != palssonKeggMapping.end())
            {
                fluxDataIterator = fluxData.find(mappingIterator->first);
                if(fluxDataIterator != fluxData.end())
                {
                    if((fluxDataIterator->second * mappingIterator->second.second) < 0)
                        tempText += mappingIterator->second.first + " BACKWARD\n";
                    else if((fluxDataIterator->second * mappingIterator->second.second) > 0)
                        tempText += mappingIterator->second.first + " FORWARD\n";
                    else
                        tempText += mappingIterator->second.first + " NONE\n";
                    ++numMapped;
                }
                else
                {
                    if(ui.comboBox->currentIndex() == 0)
                        tempText += mappingIterator->second.first + " NONE\n";
                }
                ++mappingIterator;
            }

            tempText += "\n" + string(QString().setNum(numMapped).toAscii()) + " assigned from FBA data";
        }


        if(true)
        {

            tempText +="\n\n -- Reaction user settings from heuristic reaction directions --\n";

            map<string,pair<string,int>>::iterator mappingIterator;
            T_reactionsIterator reactionsIterator;
            mappingIterator = palssonKeggMapping.begin();
            while(mappingIterator != palssonKeggMapping.end())
            {
                reactionsIterator = pool->reactions->find(mappingIterator->first);
                if(reactionsIterator != pool->reactions->end())
                {
                    if(reactionsIterator->second.equation->directionType == direction_both)
                        tempText += mappingIterator->second.first + " BOTH\n";
                    else if(reactionsIterator->second.equation->directionType == direction_left)
                    {
                        if(mappingIterator->second.second == 1)
                            tempText += mappingIterator->second.first + " BACKWARD\n";
                        else
                            tempText += mappingIterator->second.first + " FORWARD\n";
                    }
                    else
                    {
                        if(mappingIterator->second.second == 1)
                            tempText += mappingIterator->second.first + " FORWARD\n";
                        else
                            tempText += mappingIterator->second.first + " BACKWARD\n";
                    }
                }
                ++mappingIterator;
            }
        }

        ui.textEdit->append(tempText.c_str());


    }


}

void PalssonKeggConverter::on_pushButton_4_clicked()
{
    typedef boost::spirit::rule<> t_rule;

    uint_parser<unsigned, 10, 5, 5> uint5_p;

    vector<Reaction*> reactionsT;
    vector<Reaction*>::iterator reactionsTIterator;

    string tempText;

    QString fileName = QFileDialog::getOpenFileName();
    if(fileName != "")
    {
        QFile file;
        QDir::setCurrent(fileName);

        file.setFileName(fileName);
        if(file.open(QIODevice::ReadOnly)!=true)
        {
            return;
        }

        ui.lineEdit_7->setText(fileName);

        string bytes = file.readAll().constData();
        file.close();

        fluxData.clear();
        string reaction;
        double flux;

        t_rule FLUX_LINE = 
            (*(anychar_p - blank_p - eol_p))[assign_a(reaction)]
            >> +blank_p
            >> real_p[assign_a(flux)]
            >> *eol_p;

        t_rule MAIN = *(FLUX_LINE[insert_at_a(fluxData, reaction, flux)]);

        BOOST_SPIRIT_DEBUG_NODE(FLUX_LINE);
        BOOST_SPIRIT_DEBUG_NODE(MAIN);

        if(parse(bytes.c_str(), MAIN).full)
        {
            ui.lineEdit_8->setText("ok");
        }
        else
            ui.lineEdit_8->setText("error");

        ui.lineEdit_9->setText(QString().setNum(fluxData.size()));


    }
}