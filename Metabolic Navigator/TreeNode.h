#pragma once
#include <map>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "ConnectionObject.h"
#include "MetaboliteObject.h"
#include "Reaction.h"

class ConnectionObject;
class Reaction;
class MetaboliteObject;
class TreeNode;

typedef std::vector<std::pair<TreeNode*, std::vector<Reaction*>>> T_connectedNodes;
typedef std::vector<std::pair<TreeNode*, std::vector<Reaction*>>>::iterator T_connectedNodesIterator;

class TreeNode
{
public:
    TreeNode(void);
    TreeNode(const TreeNode &p)
    {
        metabolite = p.metabolite;
        prevNodes = p.prevNodes;
        nextNodes = p.nextNodes;
        distance = p.distance;
    }
    ~TreeNode(void);
    void clear(void)
    {
        prevNodes.clear();
        nextNodes.clear();
        distance = 0;
    }
    bool operator==(const TreeNode &rhs)
    {
        if(this->metabolite == rhs.metabolite)
            return true;
        return false;
    }
    bool isValid(void)
    {
        if(metabolite == 0)
            return false;
        return true;
    }
    MetaboliteObject *metabolite;
    T_connectedNodes prevNodes; // prevNode
    T_connectedNodes nextNodes; // nextNode
    unsigned int distance;
    //map<shared_ptr<ConnectionObject>, unsigned int> connections;
};