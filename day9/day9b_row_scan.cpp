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
using Positions = std::vector<Position>;

struct Stretch
{
    std::int64_t xStart;
    std::int64_t xEnd;
    auto operator<=>( const Stretch &other) const = default;
};

struct HLine
{
    std::int64_t y;
    Stretch stretch;
    auto operator<=>( const HLine &other) const = default;
};
using HLines = std::set<HLine>;

struct BoxStarter : public HLine
{
    std::set<Position> participatingPoints;

};

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

HLine PointsToHLine( const Position &previous, const Position &current)
{
    auto x1 = current.x;
    auto x2 = previous.x;

    if (x1 > x2) std::swap( x1, x2);
    return {current.y, {x1, x2}};
}

enum EventType { Untouched, IncreaseLeft, OverlapLeft, DecreaseLeft, IncreaseRight, OverlapRight, DecreaseRight, Split, Expand, Terminate};
using Event = std::tuple< EventType, HLine>;
using Events = std::set<Event>;

std::uint8_t StrongOrderingToPositiveNumber( std::strong_ordering ord)
{
    if (ord == std::strong_ordering::less) return 0;
    else if( ord == std::strong_ordering::equal) return 1;
    else if (ord == std::strong_ordering::greater) return 2;
    else throw std::runtime_error("unexpected value for strong ordering");
}

EventType Compare( const Stretch &s1, const Stretch &s2)
{
        if ( s1.xEnd < s2.xStart or s1.xStart > s2.xEnd)
        {
            return Untouched;
        }

        auto startPointRelation = s1.xStart <=> s2.xStart;
        auto endPointRelation = s1.xEnd <=> s2.xEnd;
        using namespace std;
        using SO = std::strong_ordering;
        static constexpr std::array<std::array<EventType,3>, 3> table{{
        //        \  endpoint:
        // start   \ less          equal          greater
        /* less */  {OverlapLeft,  Terminate,      Terminate},
        /* equal */ {DecreaseLeft, Terminate,      Terminate},
        /* greater*/{Split,        DecreaseRight,  OverlapRight}
        }};

        return table.at(StrongOrderingToPositiveNumber(startPointRelation)).at(StrongOrderingToPositiveNumber(endPointRelation));
}

// HLines ProcessNewLine( const HLines &hlines, const HLine &newLine)
// {
//     HLines result;
//     for ( const auto &hline : hlines)
//     {
//         switch (Compare( hline.stretch, newLine.stretch))
//         {
//             case Untouched:
//                 result.insert( hline);
//                 result.insert( newLine);
//                 break;

//             case IncreaseLeft:
//                 break;
//             case OverlapLeft:
//                 break;
//             case DecreaseLeft:
//                 break;
//             case IncreaseRight:
//                 break;
//             case OverlapRight:
//                 break;
//             case DecreaseRight:
//                 break;
//             case Split:
//                 break;
//             case Expand:
//                 break;
//             case Terminate:
//                 break;

//         }
//     }

//     return result;
// }

HLines ReadHorizontalLines( std::istream &coordinatesStream)
{
    static const std::regex coordinates{"(\\d+),(\\d+)"};

    HLines result;
    std::smatch m;
    std::string line;

    Position previous;
    Position first;
    Position max;
    Position min;
    if (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
    {
        min = max = previous = first = {std::stol(m[1]), std::stol(m[2])};
    }

    std::cout << "line: " << first.x/1000.0 << "," << first.y / 1000.0 << " ";

    while (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
    {
        Position current{std::stol(m[1]), std::stol(m[2])};

        max.x = std::max( current.x, max.x);
        max.y = std::max( current.y, max.y);
        min.x = std::min( current.x, max.x);
        min.y = std::min( current.y, max.y);

        std::cout << current.x/1000.0 << "," << current.y/1000.0 << " ";

        if (current.y == previous.y)
        {
            // we're only interested in horizontal lines.
            result.insert( PointsToHLine(current, previous));
        }
        previous = current;
    }
    //std::cout << "max/min: " << max.x << ',' << max.y << "   " << min.x << ',' << min.y << '\n';
    std::cout << '\n';
    if (first.y == previous.y)
    {
        result.insert( PointsToHLine(previous, first));
    }

    return result;
}

int main()
{
    Timer t;
    std::ifstream input{"input9.txt"};

    const auto hlines = ReadHorizontalLines( input);
    for (auto hline : hlines)
    {
        std::cout << "y = " << hline.y << ", " << hline.stretch.xStart << " -> " << hline.stretch.xEnd << '\n';
    }

    return 0;
}
