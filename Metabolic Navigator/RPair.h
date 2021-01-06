#pragma once
#include <vector>
#include <string>

class RPair
{
public:
    RPair(void);
    ~RPair(void);
    std::string text(void);
    std::string type;
    std::string leftCompound;
    std::string rightCompound;
};
