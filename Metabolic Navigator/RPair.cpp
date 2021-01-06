#include "RPair.h"

RPair::RPair(void)
{
}

RPair::~RPair(void)
{
}

std::string RPair::text(void)
{
    return leftCompound + "_" + rightCompound + " " + type;
}