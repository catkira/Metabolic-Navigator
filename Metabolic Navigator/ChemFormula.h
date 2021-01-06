#pragma once
#include <map>
#include <string>

class ChemFormula
{
public:
    ChemFormula(void);
    ~ChemFormula(void);
    void clear(void);
    bool parseFormula(std::string);
    std::string text(void);
    std::string getLastError(void);
    bool isValid(void);
    bool matches(std::string);
    std::map<std::string,unsigned int> formula;
    std::map<std::string,unsigned int>::iterator formulaIterator;
    int getNumberOfCarbons(void);
private:
    bool valid;
    std::string errorMessage;
};
