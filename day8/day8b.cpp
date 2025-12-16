#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

#include "../timer.h"
struct Position
{
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
};

Position operator-( const Position &left, const Position &right)
{
    return { left.x - right.x, left.y - right.y, left.z - right.z};
};

using Positions = std::vector<Position>;
using BoxId =  Positions::difference_type;

using Length = std::int64_t;
struct Connection
{
    Length distanceSquared;
    BoxId first;
    BoxId second;

    auto operator<=>( const Connection &) const = default;
};

using Connections = std::vector<Connection>;

using Circuit = std::unordered_set<BoxId>;
using Circuits = std::vector< Circuit>;

Positions ReadCoordinates( std::istream &coordinatesStream)
{
    static const std::regex coordinates{"(\\d+),(\\d+),(\\d+)"};

    Positions result;
    std::string line;
    std::smatch m;
    while (getline(coordinatesStream, line) and regex_match( line, m, coordinates))
    {
        result.push_back({
            stoi( m[1]),
            stoi( m[2]),
            stoi( m[3])
        });
    }

    return result;
}

/**
 * Create the cartesian product of all possbile connections. For about 1K
 * positions this is reasonable. For 1M positions it would be a bit much...
 *
 * The connections are delivered as a min heap, allowing O( log n ) extraction
 * times.
 */
Connections AllConnections( const Positions &positions)
{
    Connections result;
    for ( auto i = positions.begin(); i != positions.end(); ++i)
    {
        for (auto j = i + 1; j != positions.end(); ++j)
        {
            const auto diff = *i - *j;
            const auto distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
            result.push_back( {
                static_cast<Length>(distanceSquared),
                std::distance( positions.begin(), i),
                std::distance( positions.begin(), j)});
        }
    }

    std::ranges::make_heap(result, std::greater{});
    return result;
}

Circuits::iterator FindCircuit( Circuits &circuits, BoxId box)
{
    return std::find_if(
        circuits.begin(), circuits.end(),
        [box](const auto &circuit){ return circuit.contains( box);});
}

void MergeInto( Circuit &left, const Circuit &right, const Connection &)
{
    left.insert( right.begin(), right.end());
}

int main()
{
    Timer t;
    std::ifstream input{"input8.txt"};

    auto positions = ReadCoordinates( input);

    // create circuits with one box in each.
    Circuits circuits;
    for ( auto index = 0; index < positions.size(); ++index)
    {
        circuits.push_back({index});
    }

    constexpr auto connectionCount = 1000;
    std::uint64_t result = 0;
    auto connections = AllConnections(positions);
    auto lastConnection = connections.end();
    while (true)
    {
        auto &connection = connections.front();
        const auto firstCircuit = FindCircuit( circuits, connection.first);
        const auto secondCircuit = FindCircuit( circuits, connection.second);

        if ( firstCircuit != secondCircuit)
        {
            MergeInto( *firstCircuit, *secondCircuit, connection);
            circuits.erase( secondCircuit);
        }

        if (circuits.size() == 1)
        {
            result = positions[connection.first].x * positions[connection.second].x;
            break;
        }

        std::ranges::pop_heap( connections.begin(), connections.end(), std::greater{});
        connections.pop_back();
    }

    std::cout << "product = " << result << '\n';
    return 0;
}
