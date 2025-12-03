#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>

using Number = std::size_t;

// std::pow annoyingly returns doubles
Number TenToThePowerOf( unsigned int exponent)
{
    return std::pow( 10, exponent);
}

Number Twice( Number number, int halfSize)
{
    return number + number * TenToThePowerOf( halfSize);
}

Number CalculateLower( Number lowerBound, int halfSize)
{
    const auto power = TenToThePowerOf( halfSize);
    auto candidate = lowerBound / power;
    if (Twice(candidate, halfSize) < lowerBound) return candidate + 1;
    return candidate;
}

Number CalculateUpper( Number upperBound, int halfSize)
{
    const auto power = TenToThePowerOf( halfSize);
    auto candidate = upperBound / power;
    if (Twice( candidate, halfSize) > upperBound) return candidate - 1;
    return candidate;
}

/// determine the sum of all repeating numbers of 'size' digits between first and last.
Number CountMatches(
    int size,
    Number first,
    Number last)
{
    if (size % 2 or size == 0) return 0;

    const auto halfSize = size / 2;

    // start with a lower counter like 1010 and a higher counter like 9999
    auto lowestCounter = TenToThePowerOf( halfSize - 1);
    auto highestCounter = TenToThePowerOf( halfSize) -1;

    if (Twice( lowestCounter, halfSize) < first)
    {
        lowestCounter = CalculateLower( first, halfSize);
    }

    if (Twice( highestCounter, halfSize) > last)
    {
        highestCounter = CalculateUpper( last, halfSize);
    }

    auto rangeSum = ((highestCounter - lowestCounter + 1) * ( highestCounter + lowestCounter)) / 2;
    return Twice(rangeSum, halfSize);

}

int main()
{
    std::ifstream input{"input2.txt"};
    std::string line;

    Number sum = 0;
    while (getline(input, line, ','))
    {
        static const std::regex rangeRegex{"(\\d+)-(\\d+)\\n?"};
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

