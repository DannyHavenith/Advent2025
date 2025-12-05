#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <regex>
#include <set>
#include <utility>

using Number = std::uint64_t;
using Range = std::pair<Number, Number>;
using Ranges = std::set<Range>;

Ranges UnOverlap( Ranges ranges)
{
    Ranges result;
    for (auto currentIt = ranges.begin(); currentIt != ranges.end();++currentIt)
    {
        auto current = *currentIt;
        auto mergeCandidate = std::next( currentIt);

        // ranges are already sorted on .first, so we know that
        // mergeCandidate->first >= current.first
        while (mergeCandidate != ranges.end() and mergeCandidate->first <= current.second)
        {
            if (mergeCandidate->second > current.second)
            {
                current.second = mergeCandidate->second;
            }
            mergeCandidate = ranges.erase( mergeCandidate);
        }
        result.insert( current);
    }
    return result;
}

int main()
{
    std::ifstream input{"input5.txt"};
    std::string line;
    Ranges ranges;

    static const std::regex rangeRegex{"(\\d+)-(\\d+)"};
    std::smatch m;
    while (getline(input, line) and regex_match( line, m, rangeRegex))
    {
        ranges.emplace( std::stoul( m[1]), std::stoul(m[2]));
    }

    ranges = UnOverlap( std::move( ranges));

    std::size_t freshCount = 0;
    while (getline(input, line))
    {
        if (std::any_of(
                ranges.begin(), ranges.end(),
                [num = std::stoul(line)]( const auto &range)
                {
                    return num >= range.first and num <= range.second;
                }))
        {
            ++freshCount;
        }
    }

    std::cout << "fresh count = " << freshCount << '\n';
    return 0;
}