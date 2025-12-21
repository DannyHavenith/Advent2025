#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "../timer.h"


namespace {
    struct Position
    {
        std::int64_t x;
        std::int64_t y;
    };

    using Positions = std::vector<Position>;
    using AreaUnit = std::int64_t; // no need for the extra bit.
    using Areas = std::set<AreaUnit, std::greater<AreaUnit>>;

    Position operator-( const Position &left, const Position &right)
    {
        return { left.x - right.x, left.y - right.y};
    };

    AreaUnit Area( const Position &a, const Position &b)
    {
        const auto difference = a - b;
        using std::abs;
        return (abs( difference.x) + 1) * (abs( difference.y) + 1);
    }

    Positions ReadCoordinates( std::istream &coordinatesStream)
    {
        static const std::regex coordinates{"(\\d+),(\\d+)"};

        Positions result;
        std::string line;
        std::smatch m;
        while (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
        {
            result.push_back({
                stoi( m[1]),
                stoi( m[2])
            });
        }

        return result;
    }

    /**
     * Generate the areas of all combinations of tiles.
     *
     * I'm giving up: this one is brute force.
     */
    Areas AllAreas( const Positions &positions)
    {
        Areas result;
        for ( auto i = positions.begin(); i != positions.end(); ++i)
        {
            for (auto j = i + 1; j != positions.end(); ++j)
            {
                result.insert( {
                    Area( *i, *j),
                    std::distance( positions.begin(), i),
                    std::distance( positions.begin(), j)});
            }
        }
        return result;
    }
}

int main()
{
    Timer t;
    std::ifstream input{"input9.txt"};

    const auto areas = AllAreas( ReadCoordinates(input));

    std::cout << "largest area: " << *areas.begin() << '\n';

    return 0;
}
