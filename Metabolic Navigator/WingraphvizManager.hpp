/*
LNP
Copyright (C) 2008 Benjamin Menküc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#pragma once
#include <map>
#include <string>
#include "WinGraphviz.h"
#include <atlbase.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

template
<
    typename T_NodeAttributes
>
struct T_Node
{
    T_Node(const string& name, const T_NodeAttributes& attributes, const string compartment=""):
        name(name), attributes(attributes), compartment(compartment)
    {
    };
    string name;
    string compartment;
    T_NodeAttributes attributes;
};

template
<
    typename T_EdgeAttributes,
    typename T_NodeID
>
struct T_Edge
{
    T_Edge(const T_NodeID& sourceNodeID, const T_NodeID& destNodeID, const T_EdgeAttributes& attributes):
        source(sourceNodeID), dest(destNodeID), attributes(attributes)
    {
    }
    T_EdgeAttributes attributes;
    T_NodeID source,dest;
};



template 
<
    typename T_NodeID,
    typename T_EdgeID,
    typename T_NodeAttributes,
    typename T_EdgeAttributes
>
class WingraphvizManager
{
    struct SubgraphHierachy
    {
        string id;
        std::vector<SubgraphHierachy*> children;
        std::map<T_NodeID, T_Node<T_NodeAttributes>> nodes_;
    };


public:
    WingraphvizManager(void) : pIDOT_(NULL)
    {
        clear();

        //COM support disabled!
        //initCOMObject();
    };
    ~WingraphvizManager(void)
    {
        closeCOMObject();
    };

    void clear(void)
    {
        baseGraph = 0;
        nodes_.clear();
        edges_.clear();
        rankedGraph = false;
    }
    void addSubGraph(const string& name, const string& outside);
    //void addNode(const T_NodeID&, const string&, const T_NodeAttributes&, const string& compartment="");
    void addNode(const T_NodeID&, const string&, const T_NodeAttributes&, const string& subgraphId="");
    void addEdge(const T_EdgeID&, const T_NodeID&, const T_NodeID&, const T_EdgeAttributes&);
    void saveGraphToFile(string filename, bool applyFix=false);
    void createGraph(bool withCompartments=false, bool withBinary = true, string fileName="C:\\graph.dot", bool withPlainInfo=false, bool ranked=false);

private:
    typedef typename T_Node<T_NodeAttributes> t_Node;
    typedef typename T_Edge<T_EdgeAttributes, T_NodeID> t_Edge;
    typedef typename map<T_NodeID, t_Node> t_NodesMap;
    typedef typename map<T_EdgeID, t_Edge> t_EdgesMap;
    typedef typename t_NodesMap::iterator t_NodesMapIterator;
    typedef typename t_EdgesMap::iterator t_EdgesMapIterator;

    void findSubgraph(std::string subgraphId, SubgraphHierachy* base, SubgraphHierachy** found);
    void addGraphsToDotSource(std::string& dotSource_, SubgraphHierachy* baseGraph);
    

    void initCOMObject(void);
    void closeCOMObject(void);
    SubgraphHierachy *baseGraph;
	IDOT * pIDOT_;
	HRESULT hr_;
	CComBSTR graph_;
    string svgFile;
    string dotSource_;
    t_NodesMap nodes_;
    t_NodesMapIterator nodesIterator_;
    t_EdgesMap edges_;
    t_EdgesMapIterator edgesIterator_;

    bool rankedGraph;
};

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
initCOMObject(void)
{
    const IID IID_IDOT = {0xA1080582,0xD33F,0x486E,{0xBD,0x5F,0x58,0x19,0x89,0xA3,0xC7,0xE9}};						
    const CLSID CLSID_DOT = {0x55811839,0x47B0,0x4854,{0x81,0xB5,0x0A,0x00,0x31,0xB8,0xAC,0xEC}};

    USES_CONVERSION;

    hr_ = CoInitialize(NULL);

    if (FAILED(hr_)) {
        cout << "CoInitialize Failed: " << hr_ << "\n\n";
		return;
	}
	else {
		cout << "CoInitialize succeeded\n";
	}
		
	hr_ = CoCreateInstance(CLSID_DOT, NULL, CLSCTX_ALL, IID_IDOT, (void**)(&pIDOT_));

	if (FAILED(hr_)) 
    {
	  cout << "CoCreateInstance Failed: " << hr_ << "\n\n";
	  return;
	}
	else {
	  cout << "CoCreateInstance succeeded\n";
	}
}

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
closeCOMObject(void)
{
	CoUninitialize();
}

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
addSubGraph(const string& name, const string& outside)
{
    if(outside.empty())
    {
        baseGraph = new SubgraphHierachy();
        baseGraph->id = name;
    }
    else
    {
        if (baseGraph == 0)
            throw std::exception("need to define base graph first!");

        SubgraphHierachy* found = 0;
        findSubgraph(outside, baseGraph, &found);

        if(found == 0)
            throw std::exception("outside not found!");

        SubgraphHierachy* newGraph = new SubgraphHierachy();
        newGraph->id = name;
        found->children.push_back(newGraph);
    }
}

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
findSubgraph(std::string subgraphId, SubgraphHierachy* base, SubgraphHierachy** found)
{
    if(base->id == subgraphId)
    {
        *found = base;
        return;
    }

    else
    {
        for(unsigned int n=0;n<base->children.size();n++)
        {
            findSubgraph(subgraphId, base->children.at(n), found);
            if(*found != 0)
                return;
        }
    }
}

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
addGraphsToDotSource(std::string& dotSource_, SubgraphHierachy* baseGraph)
{
    dotSource_ += "subgraph \"cluster_";
    dotSource_ += baseGraph->id;
    dotSource_ += "\" {\n";
    dotSource_ += "label = \"" + baseGraph->id + "\";\n";

    for(unsigned int n=0;n<baseGraph->children.size();n++)
    {
        addGraphsToDotSource(dotSource_, baseGraph->children.at(n));
    }

    if(rankedGraph)
    {
        unsigned int maxRank = 0;
        nodesIterator_ = baseGraph->nodes_.begin();
        while(nodesIterator_ != baseGraph->nodes_.end())
        {
            if(maxRank < (*nodesIterator_).second.attributes.rank())
                maxRank = (*nodesIterator_).second.attributes.rank();
            ++nodesIterator_;
        }

        for(unsigned int n=0;n<=maxRank;n++)
        {
            dotSource_ += "{\n";
            dotSource_ += "rank = same;\n";

            nodesIterator_ = baseGraph->nodes_.begin();
            while(nodesIterator_ != baseGraph->nodes_.end())
            {
                if((*nodesIterator_).second.attributes.rank() == n)
                    dotSource_ +=  (*nodesIterator_).second.name + ";\n";
                ++nodesIterator_;
            }
            dotSource_ += "}\n";
        }
        
    }
    else
    {
        nodesIterator_ = baseGraph->nodes_.begin();
        while(nodesIterator_ != baseGraph->nodes_.end())
        {
            dotSource_ +=  (*nodesIterator_).second.name + ";\n";
            ++nodesIterator_;
        }
    }

    dotSource_ += "\n}\n";
}

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
createGraph(bool withCompartments, bool withBinary, string fileName, bool withPlainInfo, bool ranked)
{
    dotSource_.clear();
    string temp;

    rankedGraph = ranked;

    dotSource_ += "digraph G {\n";

    vector<pair<string,T_NodeID>> compartmentNodePairs;
    vector<pair<string,T_NodeID>>::iterator compartmentNodePairsIterator;

    set<string> compartments;
    set<string>::iterator compartmentsIterator;
    
    nodesIterator_ = nodes_.begin();
    while(nodesIterator_ != nodes_.end())
    {
        //dotSource_ +="\"";
        dotSource_ += (*nodesIterator_).second.name;// + "\" ";
        (*nodesIterator_).second.attributes.dotCode(temp);
        dotSource_ += temp;
        dotSource_ += ";\n";
        compartmentNodePairs.push_back(pair<string,T_NodeID>(
            (*nodesIterator_).second.compartment,
            (*nodesIterator_).first
            ));
        compartments.insert((*nodesIterator_).second.compartment);
        nodesIterator_++;
    }
    dotSource_ += "\n";
    

    edgesIterator_ = edges_.begin();
    while(edgesIterator_ != edges_.end())
    {
        nodesIterator_ = nodes_.find((*edgesIterator_).second.source);
        if(nodesIterator_ == nodes_.end())
        {
            // invalid graph
            throw std::exception("invalid graph");
        }
        //dotSource_ += "\"";
        dotSource_ += (*nodesIterator_).second.name;
        //dotSource_ += "\" -> \"";
        dotSource_ += " -> ";
        nodesIterator_ = nodes_.find((*edgesIterator_).second.dest);
        if(nodesIterator_ == nodes_.end())
        {
            // invalid graph
            throw std::exception("invalid graph");
        }
        dotSource_ += (*nodesIterator_).second.name;
        //dotSource_ += "\";\n";
        dotSource_ += ";\n";
        edgesIterator_++;
    }

    
    if(withCompartments)
    {
        // nested compartments
        if(baseGraph != 0)
        {
            addGraphsToDotSource(dotSource_, baseGraph);
        }

        // compartments without nesting
        else
        {
            compartmentsIterator = compartments.begin();
            while(compartmentsIterator != compartments.end())
            {
                dotSource_ += "subgraph \"cluster_";
                dotSource_ += *compartmentsIterator;
                dotSource_ += "\" {\n";
                dotSource_ += "label = \"" + *compartmentsIterator + "\";\n";

                compartmentNodePairsIterator = compartmentNodePairs.begin();
                while(compartmentNodePairsIterator != compartmentNodePairs.end())
                {
                    if((*compartmentNodePairsIterator).first == *compartmentsIterator)
                    {
                        nodesIterator_ = nodes_.find((*compartmentNodePairsIterator).second);
                        dotSource_ +=  (*nodesIterator_).second.name + ";\n";
                    }
                    ++compartmentNodePairsIterator;
                }

                dotSource_ += "\n}\n";
                ++compartmentsIterator;
            }
        }
    }

    dotSource_ += "}";

    ofstream myfile;
    myfile.open(fileName.c_str());
        myfile << dotSource_;
    myfile.close();
    USES_CONVERSION;
	//MessageBox(NULL,A2T(dotSource_.c_str()) , _T("dot-source"), MB_OK);

    if(withBinary)
    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

        bool success;
        success = CreateProcessA(NULL,
            (LPSTR)string("dot.exe " + fileName + " -Tsvg -Tplain -O").c_str(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi);
        if(success)
        {
            WaitForSingleObject( pi.hProcess, INFINITE );

            // Close process and thread handles. 
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
            cout << "Calling dot.exe succeeded\n";

            ifstream myfile2(string(fileName + ".svg").c_str(), ios::in | ios::binary);
            myfile2.seekg (0, ios::end);
            unsigned int length = myfile2.tellg();
            myfile2.seekg (0, ios::beg);
            char *buf = new char[length];
            myfile2.read(buf, length);
            svgFile = string(buf, length);
            delete [] buf;
            myfile2.close();

        }
        else
            throw std::exception("Calling dot.exe failed\n");
    }
    else
    {
        hr_ = pIDOT_->ToSvg(A2BSTR(dotSource_.c_str()),&graph_);
        if (FAILED(hr_)) 
        {
            string out = "ToSvg Failed";
            throw std::exception(out.c_str());
        }
        else 
        {
            cout << "ToSvg succeeded\n";
            svgFile = string(OLE2A(graph_));
        }
	}
}


template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
saveGraphToFile(string filename, bool applyFix)
{
    if(applyFix)    // removes Sonderzeichen -> makes it work with QtSvg4
    {
        string pattern1 = "For user";
        string pattern2 = "Pages";
        int pos1,pos2;
        pos1 = svgFile.find(pattern1, 0);
        pos2 = svgFile.find(pattern2,pos1);
        if(pos1 != string::npos && pos2 != string::npos)
            svgFile.erase(pos1, pos2-pos1);
    }

    
    ofstream myfile(filename.c_str(), ios::out | ios::binary);
    //myfile.open(filename.c_str());
    //myfile << svgFile; // does not work for binary !!
    myfile.write(svgFile.c_str(), svgFile.length());
    myfile.close();
}

/*
template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
addNode(const T_NodeID& nodeID, const string& name, const T_NodeAttributes& attributes, const string& compartment)
{
    nodes_.insert(pair<T_NodeID, t_Node>(nodeID,t_Node(name, attributes, compartment)));
}
*/

template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
addNode(const T_NodeID& nodeID, const string& name, const T_NodeAttributes& attributes, const string& subgraphId)
{
    if(subgraphId.empty() || subgraphId == "none")
        nodes_.insert(pair<T_NodeID, t_Node>(nodeID,t_Node(name, attributes, "")));
    else
    {
        nodes_.insert(pair<T_NodeID, t_Node>(nodeID,t_Node(name, attributes, "")));

        SubgraphHierachy *graph = 0;
        findSubgraph(subgraphId, baseGraph, &graph);
        if(graph!= 0)
        {
            graph->nodes_.insert(pair<T_NodeID, t_Node>(nodeID,t_Node(name, attributes, subgraphId)));
        }
        else
            throw std::exception("subgraph not found!");
    }
}


template<typename T_NodeID, typename T_EdgeID, typename T_NodeAttributes, typename T_EdgeAttributes>
void WingraphvizManager<T_NodeID, T_EdgeID, T_NodeAttributes, T_EdgeAttributes>::
addEdge(const T_EdgeID& edgeID, const T_NodeID& sourceNodeID, const T_NodeID& destNodeID, const T_EdgeAttributes& attributes)
{
    edges_.insert(pair<T_EdgeID, t_Edge>(edgeID, t_Edge(sourceNodeID, destNodeID, attributes)));
}
