#include <iostream>
#include <fstream>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <map>

#include "../timer.h"

using Tokenizer = std::sregex_token_iterator;
using LabelGraph = std::map<std::string, std::set<std::string>>;
using NodeIndex = int; // not that many nodes
using Graph = std::map<NodeIndex, std::set<NodeIndex>>;
using NodeToName = std::vector< std::string>;
using NameToNode = std::map< std::string, NodeIndex>;
using Nodes = std::vector<NodeIndex>;
using NodeFlags = std::vector<bool>;

std::tuple< Graph, NodeToName, NameToNode> ReadGraph( std::istream &input)
{
    std::string line;

    LabelGraph graphWithStrings;
    static const std::regex connections{"^([^:]+):(.*)$"};
    std::smatch m;
    while (getline( input, line) and regex_match( line, m, connections))
    {
        static const std::regex labels{"\\S+"};
        std::string outputsString = m[2];
        graphWithStrings[m[1]].insert( Tokenizer{outputsString.begin(), outputsString.end(), labels, 0}, Tokenizer{});
    }
    graphWithStrings["out"] = {};

    NodeToName nodeToName;
    NameToNode nameToNode;
    Graph graph;
    for ( const auto &[key, _] : graphWithStrings)
    {
        nodeToName.push_back( key);
        nameToNode[key] = nodeToName.size() - 1;
    }

    for ( const auto &[key, successors] : graphWithStrings)
    {
        const auto successorRange = successors | std::views::transform( [&nameToNode]( const auto &name){ return nameToNode.at( name);});
        graph[nameToNode[key]].insert(successorRange.begin(), successorRange.end());
    }

    return {graph, nodeToName, nameToNode};
}

/**
 * Sort the nodes of the graph topologically.
 *
 * We could let boost.graph do all of this, but what's the fun in that?
 */
void TopologicalSort( const Graph &graph, NodeIndex start, Nodes &sortedNodes, NodeFlags &visited)
{
    if (visited[start]) return;

    visited[start] = true;
    for ( const auto child : graph.at(start))
    {
        TopologicalSort( graph, child, sortedNodes, visited);
    }
    sortedNodes.push_back( start);
}

Nodes TopologicalSort( const Graph &graph, NodeIndex start)
{
    NodeFlags visited(graph.size(), false);
    Nodes sortedNodes;
    TopologicalSort( graph, start, sortedNodes, visited);
    std::reverse( sortedNodes.begin(), sortedNodes.end());
    return sortedNodes;
}

std::size_t CountPaths( const Graph &graph, const Nodes &sortedNodes, NodeIndex from, NodeIndex to)
{
    std::size_t sum = 0;
    std::vector<std::size_t> pathCounts(graph.size(), 0);
    pathCounts[from] = 1;
    for ( auto node : sortedNodes)
    {
        const auto pathCount = pathCounts[node];
        for (const auto successor : graph.at(node))
        {
            pathCounts[successor] += pathCount;
        }
    }
    return pathCounts[to];
}

int main()
{
    Timer t;

    std::ifstream input{"input11.txt"};

    const auto &[graph, nodeToName, nameToNode] = ReadGraph( input);

    const auto sorted = TopologicalSort( graph, nameToNode.at("you"));
    const auto pathCount = CountPaths( graph, sorted, nameToNode.at("you"), nameToNode.at("out"));

    std::cout << "total path count: " << pathCount << '\n';

    return 0;
}
