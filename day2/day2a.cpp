#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>

#include "../timer.h"

namespace {
    using Number = std::size_t;

    // std::pow annoyingly returns doubles
    Number TenToThePowerOf( unsigned int exponent)
    {
        return std::pow( 10, exponent);
    }

    /**
     * Create a number that can be written as twice the sequence if digits of the input number
     * halfSize is the number of digits of the input number.
     *
     * E.g. Twice( 12, 2) = 1212, Twice( 12, 3) = 12012, Twice( 123, 3) = 123123, etc.
     */
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

    /**
     * Calculate the sum of matches with 'size' digits that are in the range
     * [first, last]
     *
     * This function does not iterate over candidates. It uses the fact that the sum
     * of a range a + (a+1) + (a+2) + ... + b (e.g. 1 + 2 + 3 + .. + 9) can be written as:
     *     ((b-a+1)*(b+a))/2
     * If we call that SUM(a,b)
     * Then the sum of:
     *  (a + pow * a) + (a+1 + pow * (a+1)) + ... + (b + pow * b) (e.g. 11 + 22 + 33 + ... + 99, where pow = 10)
     * ...can be written as SUM(a,b) + pow * SUM(a,b)
     */
    Number SumOfMatches(
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
}

int main()
{
    Timer t;

    std::ifstream input{"input2.txt"};
    std::string line;

    Number sum = 0;
    while (getline(input, line, ','))
    {
        static const std::regex rangeRegex{"(\\d+)-(\\d+)\\n?"};
        std::smatch m;
        [[maybe_unused]] auto parsed = regex_match( line, m, rangeRegex);
        assert( parsed);

        const auto firstString = m[1];
        const auto secondString = m[2];
        const auto firstSize = firstString.length();
        const auto secondSize = secondString.length();

        for ( auto size = firstSize; size <= secondSize; ++size)
        {
            sum += SumOfMatches( size, stol( firstString), stol( secondString));
        }
    }

    std::cout << "sum = " << sum << '\n';
    return 0;
}

