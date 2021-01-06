#include "ChemFormula.h"

using namespace std;

ChemFormula::ChemFormula(void)
{
    valid=false;
}

ChemFormula::~ChemFormula(void)
{
}

int ChemFormula::getNumberOfCarbons(void)
{
    formulaIterator = formula.find("C");
    if(formulaIterator == formula.end())
        return 0;
    else
        return formula["C"];
}

void ChemFormula::clear(void)
{
    valid = false;
    formula.clear();
}

bool isNum(char byte)
{
    if(byte>='0' && byte <='9')
        return true;
    else
        return false;
}

string ChemFormula::text(void)
{
    formulaIterator = formula.begin();
    string ret("");
    if(valid==false)
        return ret;
    char buf[10];
    for(unsigned int n=0;n<formula.size();n++)
    {
        ret += (*formulaIterator).first;
        if((*formulaIterator).second != 1)
        {
            _itoa_s((*formulaIterator).second,&buf[0],10,10);
            ret += buf;
        }
        ret += string(" ");
        formulaIterator++;
    }
    return ret;
}

string ChemFormula::getLastError(void)
{
    return errorMessage;
}

bool ChemFormula::isValid(void)
{
    return valid;
}

bool ChemFormula::matches(string pattern)
{
    if(pattern.length()==0)
        return false;
    unsigned int n,val,num,elementLen,sizeLen;
    map<string,unsigned int> patternFormula;
    char buf[10];
    n=0;
    do
    {
        val=0;
        sizeLen=0;
        elementLen=1;
        if(pattern[n]==' ')
            n++;
        if(n+elementLen>=(pattern.length()))
            num=1;
        else if(isNum(pattern[n+1])==false && islower(pattern[n+1]))
        {
            elementLen=2;
        }
        if(n+elementLen >= pattern.length())
            num=1;
        else if(isNum(pattern[n+elementLen])) // has explicit number
        {
            buf[0]=pattern[n+elementLen];
            buf[1]='\0';
            sizeLen=1;
            if(n+2 < (pattern.length()))
                if(isNum(pattern[n+elementLen+1]))
                {
                    buf[1]=pattern[n+elementLen+1];
                    buf[2]='\0';
                    sizeLen=2;
                }
            num = strtol(&buf[0],0,10);
        }
        else    // default number 
            num=1;
        patternFormula[pattern.substr(n,elementLen)]=num;
        n+=elementLen+sizeLen;
    }while(n<pattern.length());

    map<string,unsigned int>::iterator formulaIterator;
    map<string,unsigned int>::iterator patternFormulaIterator;
    patternFormulaIterator = patternFormula.begin();
    
    for(unsigned int n=0;n<patternFormula.size();n++)
    {
        formulaIterator = formula.find(patternFormulaIterator->first);
        if(formulaIterator == formula.end())
            return false;
        if(formulaIterator->second != patternFormulaIterator->second)
            return false;
        patternFormulaIterator++;
    }
    return true;
}

bool ChemFormula::parseFormula(string formulaString)
{
    if(formulaString.length()==0)
        return false;
    unsigned int n,val,num,elementLen,sizeLen;
    char buf[10];
    n=0;
    valid=false;
    do
    {
        val=0;
        if(n+1 <formulaString.size())
        {
            if(formulaString[n]=='(' || formulaString[n]==')')
            {
                errorMessage = "formula " + formulaString + " contains (...)";
                return false;    
            }
        }
        sizeLen=0;
        elementLen=1;
        if(n+elementLen>=(formulaString.length()))
            num=1;
        else if(isNum(formulaString[n+1])==false && islower(formulaString[n+1]))
        {
            elementLen=2;
        }
        if(n+elementLen >= formulaString.length())
            num=1;
        else if(isNum(formulaString[n+elementLen])) // has explicit number
        {
            buf[0]=formulaString[n+elementLen];
            buf[1]='\0';
            sizeLen=1;
            if(n+2 < (formulaString.length()))
                if(isNum(formulaString[n+elementLen+1]))
                {
                    buf[1]=formulaString[n+elementLen+1];
                    buf[2]='\0';
                    sizeLen=2;
                }
            num = strtol(&buf[0],0,10);
        }
        else    // default number 
            num=1;
        formula[formulaString.substr(n,elementLen)]=num;
        n+=elementLen+sizeLen;
    }while(n<formulaString.length());
    valid=true;
    return true;
}
