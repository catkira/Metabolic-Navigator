#pragma once
#include "MetaboliteObject.h"
#include "boost/shared_ptr.hpp"
#include "Reaction.h"
#include "TreeNode.h"

class TreeNode;

class ConnectionObject
{
public:
    ConnectionObject(void);
    ~ConnectionObject(void);
    MetaboliteObject *leftMetabolite;
    MetaboliteObject *rightMetabolite;
    Reaction *reaction;
};
