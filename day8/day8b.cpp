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

using Length = long double;
struct Connection
{
    Length distance;
    BoxId first;
    BoxId second;

    auto operator<=>( const Connection &) const = default;
};

using Connections = std::set<Connection>;

using Circuit = std::set<BoxId>;
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
 * positions this is reasonable. For 1M connections it would be a bit much...
 *
 * Because Connections is a set type, the connections will be ordered by
 * increasing length.
 */
Connections AllConnections( const Positions &positions)
{
    Connections result;
    for ( auto i = positions.begin(); i != positions.end(); ++i)
    {
        for (auto j = i + 1; j != positions.end(); ++j)
        {
            const auto diff = *i - *j;
            const auto distancePowered = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
            result.insert( {
                std::sqrt( static_cast<Length>(distancePowered)),
                std::distance( positions.begin(), i),
                std::distance( positions.begin(), j)});
        }
    }
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
    for ( const auto &connection : AllConnections( positions))
    {
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
    }

    std::cout << "product = " << result << '\n';
    return 0;
}
