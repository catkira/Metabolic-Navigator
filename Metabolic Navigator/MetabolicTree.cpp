#include "MetabolicTree.h"
#include "TreeNode.h"
#include "MyAlgorithms.h"
#include "Reaction.h"
#include "Compound.h"
#include "MetaboliteObject.h"
#include <QTreeWidgetItem>

using namespace std;


MetabolicTree::MetabolicTree(
    const TreeNode& startNode,
    set<Reaction*> &reactionSet, 
    set<Compound*> &metaboliteBlackList,
    unsigned int& rpairsOption, 
    MyAlgorithms *myAlgorithms) : 
        reactionSet(reactionSet), 
        metaboliteBlackList(metaboliteBlackList),
        rpairsOption(rpairsOption), 
        myAlgorithms(myAlgorithms)
{
    numNodes_ = 0;
    numEdges_ = 0;
    MetabolicTree::startNode = new TreeNode;
    *MetabolicTree::startNode = startNode;
    graphHierachyTop = 0;
    rankedGraph = false;
    withCompartments = false;
    thread = 0;
    endReason = BTE_NONE;
    iterations = 0;
    expandStrategyIsReverse = false;
}

MetabolicTree::~MetabolicTree(void)
{
}

void CreateNetworkThread::run(void)
{
    metabolicTree.buildReactionTree(endMetabolite,endNode,maxSteps,quitIfEndMetaboliteReached);
}


void MetabolicTree::clear(void)
{
    treeNodesPool.clear();
    numNodes_ = 0;
    numEdges_ = 0;

}

T_treeNodesPoolIterator MetabolicTree::getTreeNodeFromId(std::string nodeId, TreeNode*& Node)
{
    Node = 0;
    T_treeNodesPoolIterator it = treeNodesPool.find(nodeId);
    if(it != treeNodesPool.end())
        Node = &treeNodesPool[nodeId];
    else
        throw std::exception("node not found!");
    return it;
}

void MetabolicTree::expandNode(std::string nodeId)
{
    map<string, TreeNode>::iterator tempTreeNodeIterator;

    tempTreeNodeIterator = treeNodesPool.find(nodeId);
    if(tempTreeNodeIterator == treeNodesPool.end())
        throw std::exception("node not found!");

    expandMetabolite(tempTreeNodeIterator->second, 0);
}

void MetabolicTree::deleteNode(std::string nodeId)
{
    TreeNode *nodeToDelete;
    T_treeNodesPoolIterator nodeToDeleteIterator;
    nodeToDeleteIterator = getTreeNodeFromId(nodeId, nodeToDelete);
    if(nodeToDelete == 0)
        throw std::exception("node not found!");

    if(&nodeToDeleteIterator->second == startNode)
        return;

    T_treeNodesPoolIterator treeNodesPoolIterator = treeNodesPool.begin();
    T_connectedNodesIterator connectedNodesIterator;
    while(treeNodesPoolIterator != treeNodesPool.end())
    {
        connectedNodesIterator = treeNodesPoolIterator->second.prevNodes.begin();
        while(connectedNodesIterator != treeNodesPoolIterator->second.prevNodes.end())
        {
            if(connectedNodesIterator->first == nodeToDelete)
                connectedNodesIterator = treeNodesPoolIterator->second.prevNodes.erase(connectedNodesIterator);
            else
                ++connectedNodesIterator;
        }

        connectedNodesIterator = treeNodesPoolIterator->second.nextNodes.begin();
        while(connectedNodesIterator != treeNodesPoolIterator->second.nextNodes.end())
        {
            if(connectedNodesIterator->first == nodeToDelete)
                connectedNodesIterator = treeNodesPoolIterator->second.nextNodes.erase(connectedNodesIterator);
            else
                ++connectedNodesIterator;
        }
        ++treeNodesPoolIterator;
    }

    treeNodesPool.erase(nodeToDeleteIterator);
}

void MetabolicTree::expandMetabolite(TreeNode &Node, 
    map<string,TreeNode*> *endNodes)
{
    map<MetaboliteObject *, vector<Reaction *>> targetMetaboliteLinkedReactions;
    map<MetaboliteObject *, vector<Reaction *>>::iterator targetMetaboliteLinkedReactionsIterator;
    map<Reaction *, vector<MetaboliteObject*>> targetReactionLinkedMetabolites;

    bool withEndNodes = true;
    if(endNodes == 0)
        withEndNodes = false;

    myAlgorithms->calculatePossibleNewMetabolites(Node.metabolite, &reactionSet, 
        &targetMetaboliteLinkedReactions, &targetReactionLinkedMetabolites, withCompartments, rpairsOption, expandStrategyIsReverse);

    TreeNode *tempTreeNode;
    T_treeNodesPoolIterator tempTreeNodeIterator;

    unsigned int iterationStep = Node.distance + 1;

    // connect the new nodes to the bottom of the tree and create new endpoints
    targetMetaboliteLinkedReactionsIterator = targetMetaboliteLinkedReactions.begin();
    while(targetMetaboliteLinkedReactionsIterator != targetMetaboliteLinkedReactions.end())
    {
        if(metaboliteBlackList.find(targetMetaboliteLinkedReactionsIterator->first->compound) !=
            metaboliteBlackList.end())
        {
            targetMetaboliteLinkedReactionsIterator++;
            continue;        
        }

        tempTreeNodeIterator = treeNodesPool.find(targetMetaboliteLinkedReactionsIterator->first->key());

        if(tempTreeNodeIterator != treeNodesPool.end())
        {
            // dont add new EndPoint to end of tree if it already exists
            // in this case: only create a link to the existing MetaboliteObject

            // only add connection to right metabolite for now, because we just want to step up in the tree

            // just take one possible reaction for now

            tempTreeNodeIterator->second.prevNodes.push_back
                (pair<TreeNode*,vector<Reaction*>>(&Node,targetMetaboliteLinkedReactionsIterator->second));
            Node.nextNodes.push_back
                (pair<TreeNode*,vector<Reaction*>>(&(tempTreeNodeIterator->second),
                targetMetaboliteLinkedReactionsIterator->second));
            numEdges_++;
        }
        else
        {
            // create new EndPoint and create link
            TreeNode tempTreeNode;
            //tempTreeNode->distance = iterationStep+1;
            tempTreeNode.distance = Node.distance + 1;
            tempTreeNode.metabolite = targetMetaboliteLinkedReactionsIterator->first;
            tempTreeNodeIterator = treeNodesPool.insert(pair<string,TreeNode>(tempTreeNode.metabolite->key(),tempTreeNode)).first;

            if(withEndNodes)
                endNodes->insert(pair<string,TreeNode*>(tempTreeNode.metabolite->key(),&tempTreeNodeIterator->second));
            numNodes_++;

            /*
            if(tempTreeNode->metabolite->compound == endMetabolite->compound &&
                tempTreeNode->metabolite->compartment == endMetabolite->compartment)
            {
                if(quitIfEndMetaboliteReached)
                    go=false;
                emit addLogLine("Target metabolite reached after " + QString().setNum(iterationStep) + " steps");
                *endNode = tempTreeNode;
            }
            */

            // create link
            //tempTreeNodeIterator = treeNodesPool.find(tempTreeNode->metabolite->key());

            // only add connection to right metabolite for now, because we just want to step up in the tree
            //tempTreeNodeIterator = treeNodesPool.find(targetMetaboliteLinkedReactionsIterator->first->key());

            // just take one possible reaction for now
            tempTreeNodeIterator->second.prevNodes.push_back
                (pair<TreeNode*,vector<Reaction*>>(&Node,targetMetaboliteLinkedReactionsIterator->second));
            Node.nextNodes.push_back
                (pair<TreeNode*,vector<Reaction*>>(&tempTreeNodeIterator->second,targetMetaboliteLinkedReactionsIterator->second));
            numEdges_++;

        }
        targetMetaboliteLinkedReactionsIterator++;
    }
}

void MetabolicTree::buildReactionTreeNewThread(
    MetaboliteObject *endMetabolite,
    TreeNode **endNode,
    unsigned int maxSteps, 
    bool quitIfEndMetaboliteReached)
{
    if (thread != 0)
    {
        if(thread->isRunning())
            return;
        thread->terminate();
        delete thread;
    }
    thread = new CreateNetworkThread(*this, endMetabolite, endNode, maxSteps, quitIfEndMetaboliteReached);
    endReason = BTE_RUNNING;
    thread->start();
}

void MetabolicTree::buildReactionTree(
    MetaboliteObject *endMetabolite,
    TreeNode **endNode,
    unsigned int maxSteps, 
    bool quitIfEndMetaboliteReached)
{
    unsigned int iterationStep=0;
    map<string,TreeNode*> endMetabolitesNodes;
    map<string,TreeNode*> newEndMetabolitesNodes;
    map<string,TreeNode*>::iterator endMetabolitesNodesIterator;

    treeNodesPool.clear();
    iterations = 0;

    endReason = BTE_NONE;

    treeNodesPool.insert(pair<string,TreeNode>(startNode->metabolite->key(), *startNode));

    T_treeNodesPoolIterator it =  treeNodesPool.find(startNode->metabolite->key());
    if(it != treeNodesPool.end())
    {
        startNode = &it->second;
    }
    else
        throw std::exception("");
    /*
    bool withCompartments = false;
    if(startNode->metabolite->compartment != string("none"))
        withCompartments = true;
        */

    //startNode->clear();
    startNode->nextNodes.clear();
    startNode->prevNodes.clear();
    startNode->distance = 0;
    newEndMetabolitesNodes.insert(pair<string,TreeNode*>(startNode->metabolite->key(),startNode));
    numNodes_=1;
    bool go=true;
    do
    {
        endMetabolitesNodes = newEndMetabolitesNodes;
        newEndMetabolitesNodes.clear();
        endMetabolitesNodesIterator = endMetabolitesNodes.begin();
        while(endMetabolitesNodesIterator != endMetabolitesNodes.end())
        {
        
            expandMetabolite(*endMetabolitesNodesIterator->second, &newEndMetabolitesNodes);

            endMetabolitesNodesIterator++;
        }

        if(newEndMetabolitesNodes.size()==0)
        {
            go=false;
            endReason = BTE_COMPLETE;
        }

        if((iterationStep+1) >= maxSteps)
        {
            go=false;
            endReason = BTE_MAXITERATIONS;
        }

        ++iterationStep;
        ++iterations;
        /*
        emit addLogLine(QString("Iteration ") + QString().setNum(iterationStep) + ": " +
            QString().setNum(numNodes) + " nodes(total)");
            */
    }while(go==true);
}

void MetabolicTree::createGraph(void)
{
    wingraphvizManager.clear();

    if(graphHierachyTop!=0)
    {
        addSubgraph(graphHierachyTop,0);
    }


    addTreeNodeToGraphviz(*startNode,0);


    wingraphvizManager.createGraph(true, true, "C:\\graph.dot", false, rankedGraph);

    wingraphvizManager.saveGraphToFile("C:\\metabolicNavigator.svg", true);
}

void MetabolicTree::addSubgraph(QTreeWidgetItem *graph, QTreeWidgetItem *parent)
{
    //wingraphvizManager.addSubGraph(string(graph->text(0).toAscii()), "");

    for(unsigned int n=0;n<graph->columnCount();n++)
    {
        if(parent == 0)
            wingraphvizManager.addSubGraph(string(graph->text(n).toAscii()), "");
        else
            wingraphvizManager.addSubGraph(string(graph->text(n).toAscii()), string(parent->text(0).toAscii()));
    }

    for(unsigned int n=0;n<graph->childCount();n++)
    {
        /*
        if(parent == 0)
            wingraphvizManager.addSubGraph(string(graph->text(0).toAscii()), "");
        else
            wingraphvizManager.addSubGraph(string(graph->text(0).toAscii()), string(parent->text(0).toAscii()));
            */

        addSubgraph(graph->child(n), graph);
    }
}

void MetabolicTree::addTreeNodeToGraphviz(TreeNode &node, unsigned int generation)
{ 
    unsigned int n;
    char buf[10];
    if(node.metabolite == 0)
        throw std::exception("error");

    if(node == *startNode)
    {
        graphvizNodeAttributes.setFillcolor("green");
    }
    else
        graphvizNodeAttributes.setFillcolor("");    
    graphvizNodeAttributes.setFontsize("14");    
    _itoa_s(generation,&buf[0],10,10);
    //wingraphvizManager.addNode(node.metabolite->compound->cNumber,
    /*
    wingraphvizManager.addNode(node.metabolite->key(),
        node.metabolite->key() + "\\n" + node.metabolite->compound->names[0] + "\\n" + string(buf),
        graphvizNodeAttributes, node.metabolite->compartment);
        */
    //graphvizNodeAttributes.setDisplayText(node.metabolite->key() + "\\n" + node.metabolite->compound->names.begin()->c_str() + "\\n" + string(buf));

    if(!node.metabolite->displayText.empty())
        graphvizNodeAttributes.setDisplayText(node.metabolite->key() + "\\n" + node.metabolite->displayText + "\\n" + string(buf));
    else
        graphvizNodeAttributes.setDisplayText(node.metabolite->key() + "\\n" + node.metabolite->compound->names.begin()->c_str() + "\\n" + string(buf));

    graphvizNodeAttributes.setRank(generation);

    wingraphvizManager.addNode(
        node.metabolite->key(),
        node.metabolite->key(), 
        graphvizNodeAttributes, 
        node.metabolite->compartment);

    for(n=0;n<node.nextNodes.size();n++)
    {
       wingraphvizManager.addEdge(node.metabolite->key() + node.nextNodes[n].first->metabolite->key(), 
            node.metabolite->key(), 
            node.nextNodes[n].first->metabolite->key(),
            graphvizEdgeAttributes);
        // prevent loops
        if(node.distance < node.nextNodes[n].first->distance)
        {
            addTreeNodeToGraphviz(*(node.nextNodes[n].first), generation+1);
        }
    }
}