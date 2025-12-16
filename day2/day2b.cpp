#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <regex>

#include "../timer.h"

using Number = std::uint64_t;

// std::pow annoyingly returns doubles
Number TenToThePowerOf( unsigned int exponent)
{
    return std::pow( 10, exponent);
}

/**
 * Generate a number that can be used to create a repeated sequence.
 *
 * An example of an expander with segmentSize 2 and segmentCount 3 would be:
 *    10101
 *
 * This can be used to turn a number like e.g. 13 into a number with repeated
 * digits like 131313.
 */
Number Expander( int segmentSize, int segmentCount)
{
    Number expander = 0;
    const Number power = TenToThePowerOf( segmentSize);
    for ( auto count = 0; count < segmentCount; ++count)
    {
        expander = expander * power + 1;
    }
    return expander;
}

/**
 * Calculate the lowest number, that if repeated 'segment' times would be higher
 * than or equal to the given lower bound.
 */
Number CalculateLower( Number lowerBound, int segmentSize, int segments)
{
    auto candidate = TenToThePowerOf( segmentSize - 1);
    if ( candidate * Expander( segmentSize, segments) >= lowerBound) return candidate;

    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    candidate = lowerBound / power; // take the leftmost 'segmentSize' digits of lowerBound
    if (candidate * Expander( segmentSize, segments) >= lowerBound) return candidate;

    return candidate + 1;
}

/**
 * Calculate the highest number, that if repeated 'segment' times would be
 * higher than or equal to the given upper bound.
*/
Number CalculateUpper( Number upperBound, int segmentSize, int segments)
{
    auto candidate = TenToThePowerOf( segmentSize) -1;
    if ( candidate * Expander( segmentSize, segments) <= upperBound) return candidate;

    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    candidate = upperBound / power;
    if ( candidate * Expander( segmentSize, segments) <= upperBound) return candidate;

    return candidate - 1;
}

/**
 * Count how many elements in previousSegments are factors of segment.
 */
int CountFactorsOf( int segment, const std::set<int> &previousSegments)
{
    return std::count_if(
        previousSegments.begin(), previousSegments.end(),
        [segment]( int previousSegment)
        {
            return not (segment % previousSegment);
        });
}

/**
 * Calculate the sum of of numbers that
 *   * have 'size' digits
 *   * have segmentCount repeats of the same sequence of digits
 *   * are between first and last (inclusive)
 *
 * ...and do that calculation in O(1) time. In other words, the complexity does
 * not depend on the size of the input range.
 *
 * It makes use of this formula:
 *   a + (a+1) + (a+2) + ... + b  (e.g. 19 + 20 + 21 + 22 + ...)
 *    = ((b - a + 1) * (a + b)) / 2
 *
 * Repeated sequences like for example 1919 + 2020 + 2121 + 2222 + ...
 * can be written as x * 19 + x * 20 + x * 21 + x * 22 + ...,
 * which is x * ( 19 + 20 + 21 + 22 + ...),
 * where in this example x = 101,
 *
 */
Number SumOfMatchesSegmented( int size, int segmentCount, Number first, Number last)
{
        const auto segmentSize = size / segmentCount;

        const auto lowestCounter = CalculateLower( first, segmentSize, segmentCount);
        const auto highestCounter = CalculateUpper( last, segmentSize, segmentCount);
        const auto rangeSum = ((highestCounter - lowestCounter + 1) * ( highestCounter + lowestCounter)) / 2;
        return  rangeSum * Expander( segmentSize, segmentCount);
}

/**
 * Calculate the sum of matches with 'size' digits that are in the range [first, last]
 *
 */
Number SumOfMatches(
    int size,
    Number first,
    Number last)
{
    Number sum = 0;
    std::set<int> previousSegmentCounts;

    // SegmentCount is how many repeated sequences there are in a number,
    // i.e. segmentCount 2 corresponds to a number like 123123, while
    // segmentCount 3 corresponds to a number like 121212
    for (auto segmentCount = 2; segmentCount <= size; ++segmentCount)
    {
        if (size % segmentCount) continue;

        // We have a problem that the same number may contribute to the sum of
        // different segment counts.
        //
        // For instance the number '111111' may appear in a 2-segment '111 111',
        // in a 3-segment '11 11 11' and in a 6-segment '1 1 1 1 1 1'. In this
        // example, we can recognize this because both 2 and 3 are factors of 6.
        // This means that every 6-segment number (like '222222' and '333333')
        // has already appeared in the 2-segment and the 3-segment sum. Because
        // we know that these numbers have been added twice (for 2-segments and
        // for 3-segments), we know we shouldn't add the 6-segment sum, but in
        // fact subtract it so that these numbers are counted only once.
        //
        auto thisSegmentWeight = 1 - CountFactorsOf( segmentCount, previousSegmentCounts);
        if (thisSegmentWeight == 0) continue;

        previousSegmentCounts.insert( segmentCount);

        sum += thisSegmentWeight * SumOfMatchesSegmented( size, segmentCount, first, last);
    }

    return sum;
}

int main()
{
    Timer t;
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
            sum += SumOfMatches( size, stol( firstString), stol( secondString));
        }
    }

    std::cout << "sum = " << sum << '\n';
    return 0;
}

