#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <utility>

#include "../timer.h"

namespace {
    using BeamCount = std::size_t;
    using Positions = std::set<int>;

    // This maps from (horizontal) beam position
    // to the number of beams traveling in that position.
    using Beams = std::map<int, BeamCount>;

    Positions FindAll( char needle, std::string_view haystack)
    {
        Positions result;
        for (auto i = haystack.begin(); i != haystack.end(); ++i)
        {
            if (*i == needle) result.insert( std::distance( haystack.begin(), i));
        }
        return result;
    }
}

int main()
{
    Timer t;
    std::ifstream input{"input7.txt"};
    std::string line;
    namespace ranges = std::ranges;

    getline( input, line);

    // start with 1 beam in every position marked 'S'
    Beams beams;
    ranges::transform(
        FindAll( 'S', line),
        std::inserter( beams, beams.end()),
        []( auto pos){ return std::pair{pos, 1};}
    );


    while (getline(input, line))
    {

        Positions hits;
        ranges::set_intersection(
            std::views::keys( beams),
            FindAll( '^', line),
            std::inserter( hits, hits.end()));

        for ( const auto position: hits)
        {
            auto beamCount = beams[position];
            beams.erase( position);
            beams[position - 1] += beamCount;
            beams[position + 1] += beamCount;
        }
    }

    const auto totalBeams = std::accumulate(
        beams.begin(), beams.end(),
        BeamCount{},
        []( auto count, const auto &beam){ return count + beam.second;});

    std::cout << "total Beams: " << totalBeams << '\n';

    return 0;
}
