#pragma once
#include <map>
#include <string>
#include <set>
#include <QThread>

#include "GraphvizEdgeAttributes.h"
#include "GraphvizNodeAttributes.h"
#include "WingraphvizManager.hpp"

class TreeNode;
class Compound;
class Reaction;
class MyAlgorithms;
class MetaboliteObject;
class QTreeWidgetItem;
class MetabolicTree;

const unsigned int TRANSITIONCONDITION_ANY = 0;
const unsigned int TRANSITIONCONDITION_MAIN = 1;
const unsigned int TRANSITIONCONDITION_CARBON = 2;
const unsigned int TRANSITIONCONDITION_LABEL = 3;

// for mismatch
const unsigned int TRANSITIONCONDITION_ANYPATH = (0<<8);
const unsigned int TRANSITIONCONDITION_NOPATH = (1<<8);


enum BuildTreeEndReason
{
    BTE_NONE,
    BTE_RUNNING,
    BTE_COMPLETE,
    BTE_MAXITERATIONS
};

typedef std::map<std::string,TreeNode> T_treeNodesPool;
typedef T_treeNodesPool::iterator T_treeNodesPoolIterator;


class CreateNetworkThread : public QThread
{
    Q_OBJECT
public:
    CreateNetworkThread(MetabolicTree &metabolicTree, 
        MetaboliteObject *endMetabolite,
        TreeNode **endNode,
        unsigned int maxSteps,
        bool quitIfEndMetaboliteReached)
        : 
        metabolicTree(metabolicTree), 
        endMetabolite(endMetabolite), 
        endNode(endNode), 
        maxSteps(maxSteps), 
        quitIfEndMetaboliteReached(quitIfEndMetaboliteReached)
    {
    };
    ~CreateNetworkThread(){};
    void run();
private:
    MetabolicTree &metabolicTree;
    MetaboliteObject *endMetabolite;
    TreeNode **endNode;
    unsigned int maxSteps;
    bool quitIfEndMetaboliteReached;
};

class MetabolicTree
{
public:


    MetabolicTree(
        const TreeNode& startNode,
        std::set<Reaction*> &reactionSet,
        std::set<Compound*> &metaboliteBlackList,
        unsigned int &rpairsOption,
        MyAlgorithms *myAlgorithms);

    ~MetabolicTree(void);

    void expandNode(std::string nodeId);

    void deleteNode(std::string nodeId);

    void clear(void);

    void expandMetabolite(
        TreeNode &Node, 
        std::map<std::string,TreeNode*> *endNodes);

    void buildReactionTree(
        MetaboliteObject *endMetabolite,
        TreeNode **endNode,
        unsigned int maxSteps, 
        bool quitIfEndMetaboliteReached);

    void buildReactionTreeNewThread(
        MetaboliteObject *endMetabolite,
        TreeNode **endNode,
        unsigned int maxSteps, 
        bool quitIfEndMetaboliteReached);

    void createGraph(void);

    void setSubgraphHierachy(QTreeWidgetItem *twi)
    {
        graphHierachyTop = twi;
    }

    void setExpandstrategyToReverse(bool es){expandStrategyIsReverse = es;}

    T_treeNodesPoolIterator getTreeNodeFromId(std::string nodeId,  TreeNode*& Node);

    unsigned int numNodes(void){return numNodes_;};
    unsigned int numEdges(void){return numEdges_;};

    TreeNode *startNode;
    T_treeNodesPool treeNodesPool;

    bool rankedGraph;
    bool withCompartments;

    BuildTreeEndReason endReason;

    unsigned int iterations;

private:


    class CreateNetworkThread *thread;

    void addTreeNodeToGraphviz(TreeNode &node, unsigned int generation);
    void addSubgraph(QTreeWidgetItem *, QTreeWidgetItem *);

    WingraphvizManager<string, string, GraphvizNodeAttributes, GraphvizEdgeAttributes> wingraphvizManager;
    GraphvizNodeAttributes graphvizNodeAttributes;
    GraphvizEdgeAttributes graphvizEdgeAttributes;

    QTreeWidgetItem *graphHierachyTop;

    unsigned int numNodes_;
    unsigned int numEdges_;
    std::set<Reaction*> reactionSet;
    unsigned int rpairsOption;
    std::set<Compound*> metaboliteBlackList;
    bool expandStrategyIsReverse;

    MyAlgorithms *myAlgorithms;

};
