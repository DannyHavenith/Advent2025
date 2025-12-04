#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <regex>

using Number = std::uint64_t;

// std::pow annoyingly returns doubles
Number TenToThePowerOf( unsigned int exponent)
{
    return std::pow( 10, exponent);
}

/**
 * Expand a number into a repeated sequence, e.g. expand the number 12 into
 * 121212 (if segments is 3 and segmentsize = 2)
 */
Number Expand( Number number, int segmentSize, int segments)
{
    Number sum = 0;
    for ( auto count = 0; count < segments; ++count)
    {
        sum += TenToThePowerOf( count * segmentSize) * number;
    }
    return sum;
}

/**
 * Calculate the lowest number, that if repeated 'segment' times would be higher
 * than or equal to the given lower bound.
 */
Number CalculateLower( Number lowerBound, int segmentSize, int segments)
{
    auto candidate = TenToThePowerOf( segmentSize - 1);
    if (Expand( candidate, segmentSize, segments) >= lowerBound) return candidate;

    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    candidate = lowerBound / power;
    if (Expand(candidate, segmentSize, segments) >= lowerBound) return candidate;

    return candidate + 1;
}

/**
 * Calculate the highest number, that if repeated 'segment' times would be
 * higher than or equal to the given upper bound.
*/
Number CalculateUpper( Number upperBound, int segmentSize, int segments)
{
    auto candidate = TenToThePowerOf( segmentSize) -1;
    if (Expand( candidate, segmentSize, segments) <= upperBound) return candidate;

    const auto power = TenToThePowerOf( segmentSize * (segments-1));
    candidate = upperBound / power;
    if (Expand( candidate, segmentSize, segments) <= upperBound) return candidate;

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
 * Calculate the sum of matches with 'size' digits that are in the range [first, last]
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

        const auto segmentSize = size / segmentCount;


        const auto lowestCounter = CalculateLower( first, segmentSize, segmentCount);
        const auto highestCounter = CalculateUpper( last, segmentSize, segmentCount);
        // Note that we're not iterating over candidate numbers here. If for example segmentCount is 3 and
        // lowestcounter = 21 and highestcounter = 32 then this formula will calculate the sum of all numbers
        //     212121 + 222222 + 232323 + ... + 313131 + 323232
        const auto rangeSum = ((highestCounter - lowestCounter + 1) * ( highestCounter + lowestCounter)) / 2;
        sum += thisSegmentWeight * Expand(rangeSum, segmentSize, segmentCount);
    }
    return sum;
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
            sum += SumOfMatches( size, stol( firstString), stol( secondString));
        }
    }

    std::cout << "sum = " << sum << '\n';
    return 0;
}

