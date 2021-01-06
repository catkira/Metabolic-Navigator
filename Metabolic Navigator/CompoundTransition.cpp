#include "CompoundTransition.h"
//#include "CompoundDefinition.h"
#include "Compound.h"
#include <sstream>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "spirit_include.h"
#include <QString>
#include "Polynom.h"

using namespace std;
using namespace boost::spirit;

//using namespace boost::numeric::ublas;

std::vector<std::pair<int,int>> CompoundTransition::permutationSequence;
std::vector<int> CompoundTransition::sequenceLength;

CompoundTransition::CompoundTransition(void)
{
    transitionMatrix = 0;
    num1=num2=-1;

    if(permutationSequence.empty())
    {
        permutationSequence.push_back(pair<int,int>(1,1));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        permutationSequence.push_back(pair<int,int>(6,7));
        add6homotopesToPermutationSequence();

        // generate higher orders dynamically

        sequenceLength.push_back(0);
        sequenceLength.push_back(0);
        sequenceLength.push_back(2); // 2
        sequenceLength.push_back(6); // 3
        sequenceLength.push_back(24); // 4
        sequenceLength.push_back(120); // 5
        sequenceLength.push_back(720); // 6
        sequenceLength.push_back(5040); // 7

        sequenceLength.push_back(1); // 8 (8! is very big! not yet implemented)
    }
}

void CompoundTransition::add6homotopesToPermutationSequence(void)
{
    // 719 entries

    add5homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(5,6));
    add5homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(5,6));
    add5homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(5,6));
    add5homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(5,6));
    add5homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(5,6));
    add5homotopesToPermutationSequence();
}

void CompoundTransition::add5homotopesToPermutationSequence(void)
{
    //119 entries

    add4homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(4,5));
    add4homotopesToPermutationSequence();
    
    permutationSequence.push_back(pair<int,int>(4,5));
    add4homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(4,5));
    add4homotopesToPermutationSequence();

    permutationSequence.push_back(pair<int,int>(4,5));
    add4homotopesToPermutationSequence();

}

void CompoundTransition::add4homotopesToPermutationSequence(void)
{
        // 23 entries

        permutationSequence.push_back(pair<int,int>(1,2));

        // ------- 3 homotopes ---------
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));

        // ------- 4 homotopes ---------
        permutationSequence.push_back(pair<int,int>(3,4));
        
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));

        permutationSequence.push_back(pair<int,int>(3,4));
        
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));

        permutationSequence.push_back(pair<int,int>(3,4));
        
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));
        permutationSequence.push_back(pair<int,int>(2,3));
        permutationSequence.push_back(pair<int,int>(1,2));


}

CompoundTransition::~CompoundTransition(void)
{
    delete transitionMatrix;
}


struct AddEntry2
{
    AddEntry2(CompoundTransition& self) : self(self)
    {
    }
    template <typename IteratorT>
    void operator()(IteratorT first, IteratorT last) const
    {
        self.rawInformation.push_back(pair<int,int>(self.num1,self.num2));
    }
    CompoundTransition& self;
};


void CompoundTransition::parseFromKeggRPair(std::string text)
{
    boost::spirit::parse_info<> parseInfo;
    typedef boost::spirit::rule<> t_rule;
    uint_parser<unsigned, 10, 5, 5> uint5_p;



    AddEntry2 addEntry(*this);


    t_rule ALIGN = 
        str_p("ALIGN")
        >> +blank_p
        >>int_p
        >>eol_p;

    t_rule NUMBERSYMBOL1 = 
        int_p[assign_a(num1)]
        >>ch_p(':')
        >>+(anychar_p - blank_p - eol_p);

    t_rule NUMBERSYMBOL2 = 
        int_p[assign_a(num2)]
        >>ch_p(':')
        >>+(anychar_p - blank_p - eol_p);

    t_rule ALIGNLINE = 
        +blank_p
        >>(int_p | ch_p('-'))
        >>+blank_p
        >>(NUMBERSYMBOL1 ^ ch_p('*')[assign_a(num1,-1)])
        >>+blank_p
        >>((NUMBERSYMBOL2 ^ ch_p('*')[assign_a(num1,-1)]))[addEntry]
        >>*(+blank_p>>*(anychar_p-eol_p))
        >>eol_p;

    t_rule MAIN =
        ALIGN
        >> *(ALIGNLINE);

    BOOST_SPIRIT_DEBUG_NODE(ALIGN);
    BOOST_SPIRIT_DEBUG_NODE(ALIGNLINE);
    BOOST_SPIRIT_DEBUG_NODE(NUMBERSYMBOL1);
    BOOST_SPIRIT_DEBUG_NODE(NUMBERSYMBOL2);
    BOOST_SPIRIT_DEBUG_NODE(MAIN);

    rawInformation.clear();



    if(!parse(text.c_str(),MAIN).full)
        throw std::exception("syntax error in transition description!");
//    rawInformation.push_back(std::pair<int,int>(num1,num2));
    base = "kegg";
}

void CompoundTransition::setupMatrix(void)
{
    if(compoundPair.first == 0 || compoundPair.second == 0)
        throw std::exception("can not create matrix, because no compound defined!");

    setupMatrix(compoundPair.second->formula.getNumberOfCarbons(), compoundPair.first->formula.getNumberOfCarbons());
}

void CompoundTransition::setupMatrix(unsigned int n, unsigned int m)
{
    if(transitionMatrix != 0)
        delete transitionMatrix;
    if(m==0 || n==0)
        throw std::exception("Dimension of transition matrix can not be 0!");
    transitionMatrix = new T_matrix(n,m);
    transitionMatrix->clear();


    boost::numeric::ublas::matrix<Polynome> *transitionMatrix2;
/*
    transitionMatrix2 = new matrix<Polynome>(n,m);

 
    transitionMatrix2->operator()(0,0)=Polynome("1");

    stringstream stream;
    stream << *transitionMatrix2;
    //cout << *transitionMatrix;
    QString temp = stream.str().c_str();
    */
}

void CompoundTransition::createPermutatedMatrixRow(T_matrix& m, const int& index1, const int& index2)
{
    using namespace boost::numeric::ublas;

    boost::numeric::ublas::vector<double> tempRow;
    matrix_row<matrix<double> > row1(m,index1);
    matrix_row<matrix<double> > row2(m,index2);
    tempRow = row1;
    row1 = row2;
    row2 = tempRow;
}

void CompoundTransition::createPermutatedMatrixCol(T_matrix& m, const int& index1, const int& index2)
{
    using namespace boost::numeric::ublas;
     
    boost::numeric::ublas::vector<double> tempCol;
    matrix_column<matrix<double> > col1(m,index1);
    matrix_column<matrix<double> > col2(m,index2);
    tempCol = col1;
    col1 = col2;
    col2 = tempCol;
}


void CompoundTransition::calculateMatrix(void)
{
    T_rawInformationIterator rawInformationIterator;
    rawInformationIterator = rawInformation.begin();

    try
    {
    while(rawInformationIterator != rawInformation.end())
    {
        transitionMatrix->operator ()(rawInformationIterator->second-1,rawInformationIterator->first-1)=1;
        ++rawInformationIterator;
    }
    }
    catch(std::exception &e)
    {
        string temp = e.what();
    }

    T_matrix *tempMatrix = new T_matrix(transitionMatrix->size1(),transitionMatrix->size2());

    double tempVar;
    int index1,index2;

    vector<vector<int>>::const_iterator homotopesIterator;//,homotopesIterator2;

    if(!compoundPair.first->homotopes.empty())
    {
        unsigned int totalCount = 0;
        *tempMatrix = *transitionMatrix;
        transitionMatrix->clear();
        homotopesIterator = compoundPair.first->homotopes.begin();              
        unsigned int pos=1,posOutside=1;

        while(homotopesIterator != compoundPair.first->homotopes.end())
        {
            for(unsigned int n=0;n<sequenceLength.at(homotopesIterator->size());n++)
            {
                index1 = permutationSequence[n].first;
                index2 = permutationSequence[n].second;
                index1 = homotopesIterator->at(index1-1); // translate standard permutation indices to atom numbers
                index2 = homotopesIterator->at(index2-1);
                createPermutatedMatrixCol(*tempMatrix, index1-1,index2-1);

                transitionMatrix->operator += (*tempMatrix);
                ++totalCount;             
            }
            ++homotopesIterator;
        };
        if(totalCount != 0)
            transitionMatrix->operator /= (totalCount);
    }


    if(!compoundPair.second->homotopes.empty())
    {
        unsigned int totalCount = 0;
        *tempMatrix = *transitionMatrix;
        transitionMatrix->clear();
        homotopesIterator = compoundPair.second->homotopes.begin();             
        unsigned int pos=1,posOutside=1;

        while(homotopesIterator != compoundPair.second->homotopes.end())
        {
            for(unsigned int n=0;n<sequenceLength.at(homotopesIterator->size());n++)
            {
                index1 = permutationSequence[n].first;
                index2 = permutationSequence[n].second;
                index1 = homotopesIterator->at(index1-1);
                index2 = homotopesIterator->at(index2-1);
                createPermutatedMatrixRow(*tempMatrix, index1-1,index2-1);

                transitionMatrix->operator += (*tempMatrix);
                ++totalCount;             
            }
            ++homotopesIterator;
        };
        transitionMatrix->operator /= (totalCount);
    }
    delete tempMatrix;
}

void CompoundTransition::generateRPairsID(std::string& text)
{
    text = compoundPair.first->cNumber + "_" + compoundPair.second->cNumber;
    return;
}

void CompoundTransition::generateIDText(std::string& text)
{
    if(compoundPair.first == 0 || compoundPair.second == 0)
    {
        text = "invalid rpair!";
        return;
    }

    text = compoundPair.first->getName() + " <-> " + compoundPair.second->getName();
}



void CompoundTransition::generateText(std::string& text)
{
    if(transitionMatrix == 0)
    {
        text = "";
        return;
    }
    stringstream stream;
    stream << *transitionMatrix;
    //cout << *transitionMatrix;
    QString temp = stream.str().c_str();
    temp = temp.replace("]","]\n");
    temp = temp.replace("),",")\n");
    temp = temp.replace("((","(");
    temp = temp.replace("))",")");
    text = temp.toAscii();
}
