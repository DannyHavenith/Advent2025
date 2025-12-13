#include <algorithm>
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


struct Position
{
    std::int64_t x;
    std::int64_t y;
};

Position operator-( const Position &left, const Position &right)
{
    return { left.x - right.x, left.y - right.y};
};

using Positions = std::vector<Position>;

struct Stretch
{
    std::int64_t start;
    std::int64_t end;
    auto operator<=>( const Stretch &other) const = default;
};

struct HLine
{
    std::int64_t y;
    Stretch stretch;
    auto operator<=>( const HLine &other) const = default;
};

struct VLine
{
    std::int64_t x;
    Stretch stretch;
    auto operator<=>( const VLine &other) const = default;
};

using HLines = std::set<HLine>;
using VLines = std::set<VLine>;

using AreaUnit = std::int64_t; // no need for the extra bit.
using Areas = std::set<AreaUnit, std::greater<AreaUnit>>;

AreaUnit Area( const Position &a, const Position &b)
{
    const auto difference = a - b;
    using std::abs;
    return (abs( difference.x) + 1) * (abs( difference.y) + 1);
}

HLine PointsToHLine( const Position &previous, const Position &current)
{
    auto x1 = current.x;
    auto x2 = previous.x;

    if (x1 > x2) std::swap( x1, x2);
    return { .y = current.y, .stretch = {x1, x2}};
}

VLine PointsToVLine( const Position &previous, const Position &current)
{
    auto y1 = current.y;
    auto y2 = previous.y;

    if (y1 > y2) std::swap( y1, y2);
    return { .x = current.x, .stretch = {y1, y2}};
}

std::tuple<Positions, HLines, VLines> ReadCoordinates( std::istream &coordinatesStream)
{
    static const std::regex coordinates{"(\\d+),(\\d+)"};

    HLines hlines;
    VLines vlines;
    Positions positions;

    std::smatch m;
    std::string line;

    Position previous;
    Position first;

    if (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
    {
        first = previous = {std::stol(m[1]), std::stol(m[2])};
        positions.push_back( previous);
    }

    while (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
    {
        Position current{std::stol(m[1]), std::stol(m[2])};
        positions.push_back( current);

        if (current.y == previous.y)
        {
            // we're only interested in horizontal lines.
            hlines.insert( PointsToHLine(current, previous));
        }
        else
        {
            vlines.insert( PointsToVLine(current, previous));
        }
        previous = current;
    }

    if (previous.y == first.y)
    {
        // we're only interested in horizontal lines.
        hlines.insert( PointsToHLine(first, previous));
    }
    else
    {
        vlines.insert( PointsToVLine(first, previous));
    }


    return {positions, hlines, vlines};
}

bool NoIntersections( Position a, Position b, const HLines &hlines, const VLines &vlines)
{
    // normalize so that a becomes top left and b becomes bottom right
    if (a.x > b.x) std::swap( a.x, b.x);
    if (a.y > b.y) std::swap( a.y, b.y);

    return std::ranges::all_of(
                hlines,
                [a,b](const auto &hline)
                {
                    return hline.y <= a.y or hline.y >= b.y or
                            hline.stretch.end <= a.x or
                            hline.stretch.start >= b.x;
                })
            and
            std::ranges::all_of(
                vlines,
                [a,b](const auto &vline)
                {
                    return
                        vline.x <= a.x or vline.x >= b.x or
                        vline.stretch.end <= a.y or
                        vline.stretch.start >= b.y;
                });
}

/**
 * Generate the areas of all combinations of tiles that do not intersect other
 * lines of the polygon.
 *
 * This is not the most optimal implementation. An attempt at a row-scanning
 * implementation is in day9b_row_scan.cpp, but I'm implementing this because I
 * want my star NOW!
 */
Areas AllAreas( const Positions &positions, const HLines &hlines, const VLines &vlines)
{
    Areas result;
    for ( auto i = positions.begin(); i != positions.end(); ++i)
    {
        for (auto j = i + 1; j != positions.end(); ++j)
        {
            if (NoIntersections( *i, *j, hlines, vlines))
            {
                result.insert(
                    Area( *i, *j)
                    );
            }
        }
    }
    return result;
}

int main()
{
    Timer t;
    std::ifstream input{"input9.txt"};

    const auto [positions, hlines, vlines] = ReadCoordinates( input);
    const auto largestArea = *AllAreas( positions, hlines, vlines).begin();

    std::cout << "largest enclosed area: " << largestArea << '\n';

    return 0;
}
