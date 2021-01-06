#pragma once
#include <vector>
#include <string>
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/vector.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "Compound.h"

class Reaction;
class Compound;
class TransitionMatrix;
class CompoundDefinition;

typedef std::vector<std::pair<int,int>>::iterator T_rawInformationIterator;
typedef boost::numeric::ublas::matrix<double> T_matrix;

class CompoundTransition
{
public:
    CompoundTransition(void);
    CompoundTransition(const CompoundTransition& ct)
    {
        compoundPairID = ct.compoundPairID;
        compoundPair = ct.compoundPair;
        inReactionsID = ct.inReactionsID;
        base = ct.base;
        rawInformation = ct.rawInformation;
        permutationSequence = ct.permutationSequence;
        sequenceLength = ct.sequenceLength;
        type = ct.type;

        if(ct.transitionMatrix == 0)
            transitionMatrix = 0;
        else
        {
            transitionMatrix = new T_matrix
                (ct.transitionMatrix->size1(), ct.transitionMatrix->size2());
            *(transitionMatrix) = *(ct.transitionMatrix);
        }
    }
    ~CompoundTransition(void);
    void parseFromKeggRPair(std::string text);
    void setupMatrix(unsigned int n, unsigned int m);
    void setupMatrix(void);
    void calculateMatrix();
    void generateText(std::string& text);
    void generateIDText(std::string& text);
    void generateRPairsID(std::string& text);

    std::pair<std::string, std::string> compoundPairID;
    std::pair<Compound*, Compound*> compoundPair;

    //TransitionMatrix *matrix;
    std::vector<std::pair<int,int>> rawInformation;
    std::string base;
    std::vector<std::string> inReactionsID;
    std::string type;

    T_matrix *transitionMatrix;
private:
    int calculateFaculty(int n)
    {
        int temp=0;
        for(unsigned int m=0;m<n;m++)
            temp *= m;
        return temp;
    };
    void createPermutatedMatrixRow(T_matrix& m, const int& index1, const int& index2);
    void createPermutatedMatrixCol(T_matrix& m, const int& index1, const int& index2);
    void add4homotopesToPermutationSequence(void);
    void add5homotopesToPermutationSequence(void);
    void add6homotopesToPermutationSequence(void);


    int num1,num2;

    static std::vector<std::pair<int,int>> permutationSequence;
    static std::vector<int> sequenceLength;
    

friend class AddEntry2;

};
