#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <regex>

using Number = std::uint64_t;

// std::pow annoyingly returns doubles
Number TenToThePowerOf( unsigned int exponent)
{
    return std::pow( 10, exponent);
}

Number Repeated( Number number, int segmentSize, int segments)
{
    Number sum = 0;
    for ( auto count = 0; count < segments; ++count)
    {
        sum += TenToThePowerOf( count * segmentSize) * number;
    }
    return sum;
}

Number CalculateLower( Number lowerBound, int segmentSize, int segments)
{
    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    auto candidate = lowerBound / power;
    if (Repeated(candidate, segmentSize, segments) < lowerBound) return candidate + 1;
    return candidate;
}

Number CalculateUpper( Number upperBound, int segmentSize, int segments)
{
    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    auto candidate = upperBound / power;
    if (Repeated( candidate, segmentSize, segments) > upperBound) return candidate - 1;
    return candidate;
}

/// count all repeating numbers of size 'size' between first and last.
Number CountMatches(
    int size,
    Number first,
    Number last)
{
    Number sum = 0;
    std::set<Number> matches;
    for (auto segments = 2; segments <= size; ++segments)
    {
        if (size % segments or size == 0) continue;

        const auto segmentSize = size / segments;

        // start with a lower counter like 1010 and a higher counter like 9999
        auto lowestCounter = TenToThePowerOf( segmentSize - 1);
        auto highestCounter = TenToThePowerOf( segmentSize) -1;

        if (Repeated( lowestCounter, segmentSize, segments) < first)
        {
            lowestCounter = CalculateLower( first, segmentSize, segments);
        }

        if (Repeated( highestCounter, segmentSize, segments) > last)
        {
            highestCounter = CalculateUpper( last, segmentSize, segments);
        }

        // there's a faster way, but let's do this one for now.
        for ( auto n = lowestCounter; n <= highestCounter; ++n)
        {
            matches.insert(Repeated( n, segmentSize, segments));
        }
    }
    return std::accumulate( matches.begin(), matches.end(), Number{});
}

int main()
{
    std::ifstream input{"input2.txt"};
    std::string line;

    Number sum = 0;
    while (getline(input, line, ','))
    {
        static const std::regex rangeRegex{"(\\d+)-(\\d+)\n?"};
        std::smatch m;
        auto parsed = regex_match( line, m, rangeRegex);
        assert( parsed);

        const auto firstString = m[1];
        const auto secondString = m[2];
        const auto firstSize = firstString.length();
        const auto secondSize = secondString.length();

        for ( auto size = firstSize; size <= secondSize; ++size)
        {
            sum += CountMatches( size, stol( firstString), stol( secondString));
        }
    }

    std::cout << "sum = " << sum << '\n';
    return 0;
}

