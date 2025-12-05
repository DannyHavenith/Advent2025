#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <numeric>
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

Number SumRanges( const Ranges &ranges)
{
    return std::accumulate(
        ranges.begin(), ranges.end(),
        Number{},
        []( Number value, const auto &range)
        {
            return value + range.second - range.first + 1;
        });
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

    auto freshCount = SumRanges( ranges);

    std::cout << "fresh count = " << freshCount << '\n';
    return 0;
}
