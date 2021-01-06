#pragma once
#include <string>
#include <vector>

class Compound;

class FateMapCompound
{
public:
    FateMapCompound(void);
    ~FateMapCompound(void);

    std::string inchi;
    std::string inchiAux;
    std::string inchiCoordinates;
    std::string cnumber;
    std::vector<std::string> names;
    //std::vector<std::string> equivalents;
    //std::vector<std::string> prochirales;
    std::vector<std::vector<int>> homotopes;
    std::vector<std::vector<int>> prochirales;
    Compound *compound;
    unsigned int numberOfCarbons;
};
