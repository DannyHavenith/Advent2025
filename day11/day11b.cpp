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

// Graph is an adjacency list, or in other words: for each node
// it contains the set of successor nodes.
using Graph = std::vector<std::set<NodeIndex>>;
using NameToNode = std::map< std::string, NodeIndex>;
using Nodes = std::vector<NodeIndex>;
using NodeFlags = std::vector<bool>;

std::tuple< Graph, NameToNode> ReadGraph( std::istream &input)
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

    NameToNode nameToNode;
    NodeIndex nodeCounter = 0;
    for ( const auto &[key, _] : graphWithStrings)
    {
        nameToNode[key] = nodeCounter++;
    }

    Graph graph(nameToNode.size());
    for ( const auto &[key, successors] : graphWithStrings)
    {
        const auto successorRange = successors | std::views::transform( [&nameToNode]( const auto &name){ return nameToNode.at( name);});
        graph.at(nameToNode[key]).insert(successorRange.begin(), successorRange.end());
    }

    return {graph, nameToNode};
}

/**
 * Sort the nodes of the graph that are reachable through 'start' topologically.
 * After sorting, all successor nodes of a node appear BEFORE that node in sortedNodes.
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

/**
 * Sort the nodes topologically.
 *
 * This returns all nodes reachable from 'start', ordered such that all
 * successors of a node appear AFTER that node in the result.
 */
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

    const auto &[graph, nameToNode] = ReadGraph( input);

    const auto sorted = TopologicalSort( graph, nameToNode.at("svr"));

    // either fft->dac paths are zero or dac->fft paths are zero in a DAG
    const auto fromFftToDac = CountPaths( graph, sorted, nameToNode.at("fft"), nameToNode.at("dac"));
    const auto fromDacToFft = CountPaths( graph, sorted, nameToNode.at("dac"), nameToNode.at("fft"));

    std::size_t pathCount = 0;
    if (not fromDacToFft)
    {
        pathCount = CountPaths( graph, sorted, nameToNode.at("svr"), nameToNode.at("fft"))
                    * fromFftToDac
                    * CountPaths( graph, sorted, nameToNode.at("dac"), nameToNode.at("out"));
    }
    else
    {
        pathCount = CountPaths( graph, sorted, nameToNode.at("svr"), nameToNode.at("dac"))
                    * fromDacToFft
                    * CountPaths( graph, sorted, nameToNode.at("fft"), nameToNode.at("out"));
    }

    std::cout << "total path count through dac and fft: " << pathCount << '\n';

    return 0;
}
