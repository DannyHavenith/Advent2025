#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>

using BeamCount = std::size_t;
using Positions = std::set<int>;
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

Positions MapKeys( const Beams &beams)
{
    Positions result;
    std::transform(
        beams.begin(), beams.end(),
        std::inserter( result, result.end()),
        [](const auto &pair){ return pair.first;}
    );
    return result;
}

int main()
{
    std::ifstream input{"input7.txt"};
    std::string line;

    getline( input, line);

    auto positions = FindAll( 'S', line);
    Beams beams;

    std::transform(
        positions.begin(), positions.end(),
        std::inserter( beams, beams.end()),
        []( auto pos){ return std::pair{pos, 1};}
    );


    while (getline(input, line))
    {
        const auto splitters = FindAll( '^', line);

        Positions hits;
        const auto beamPositions = MapKeys( beams); // no C++20 ranges on this compiler?

        std::set_intersection(
            beamPositions.begin(), beamPositions.end(),
            splitters.begin(), splitters.end(),
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
