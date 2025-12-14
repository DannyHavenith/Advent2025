#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "../timer.h"

using Positions = std::set<int>;
using Bitset = std::uint16_t; // max number of lights appears to be 10
using Button = Bitset;
using Buttons = std::vector<Button>;
using Tokenizer = std::sregex_token_iterator;
using Count = std::int64_t;
using Counters = std::vector< Count>;
using ButtonSequenceCache = std::map< Button, std::optional<std::vector<Buttons>>>;

static constexpr Count noResult = 1'000'000;

Positions FindAll( char needle, std::string_view haystack)
{
    Positions result;
    for (auto i = haystack.begin(); i != haystack.end(); ++i)
    {
        if (*i == needle) result.insert( std::distance( haystack.begin(), i));
    }
    return result;
}

Bitset StringToLights( const std::string &input)
{
    Bitset lights = 0;
    for (const auto c : input | std::views::reverse)
    {
        lights = 2 * lights + (c == '#');
    }
    return lights;
}

Buttons StringToButtons( const std::string &input)
{
    Buttons result;

    static const std::regex bitsequence{R"(\(([\d+,)]+)\))"};
    for (
        auto numbersSequence = Tokenizer( input.begin(), input.end(), bitsequence, 1);
        numbersSequence != Tokenizer{};
        ++numbersSequence)
    {
        static const std::regex digitsSequence{"(\\d+)"};
        Bitset button{};
        std::string numbers = *numbersSequence;
        for (
            auto bitPosition = Tokenizer{ numbers.begin(), numbers.end(), digitsSequence, 1};
            bitPosition != Tokenizer{};
            ++bitPosition)
        {
            button |= 1 << std::stoi(*bitPosition);
        }

        result.push_back( button);
    }

    return result;
}

Counters StringToCounters( const std::string &numbers)
{
    Counters result;
    static const std::regex digitsSequence{"(\\d+)"};
    std::transform(
        Tokenizer{ numbers.begin(), numbers.end(), digitsSequence, 1}, Tokenizer{},
        back_inserter( result),
        []( const auto &numberString){ return std::stoll(numberString);}
    );

    return result;
}

Counters Subtract( Counters counters, Button button)
{
    for (auto index = 0; button != 0; button >>= 1, index++)
    {
        if (button % 2) --counters[index];
    }
    return counters;
}

bool AnyLessThanZero( const Counters &counters)
{
    return std::ranges::any_of( counters, [](const auto number){return number < 0;});
}

bool AllZero( const Counters &counters)
{
    return std::ranges::all_of( counters, [](const auto number){return number == 0;});
}


// let's do the brute force thing again.
std::vector<Buttons> FindButtonSequences( Bitset lights, Buttons::const_iterator begin, Buttons::const_iterator end, Buttons buttonsSoFar = {})
{
    if (begin == end)
    {
        if (lights) return {};
        else return {buttonsSoFar};
    }

    auto solution1 = FindButtonSequences( lights, std::next( begin), end, buttonsSoFar);

    buttonsSoFar.push_back( *begin);
    const auto solution2 = FindButtonSequences( lights ^ *begin, std::next( begin), end, buttonsSoFar);
    solution1.insert(solution1.end(), solution2.begin(), solution2.end());

    return solution1;
}


// Create a bitmask that represents all counters that have an odd value
// At the same time, decrease those counters' values by 1 so that all
// values are even.
Bitset CountersToLights( const Counters &counters)
{
    Bitset bitset{};

    for (auto &counter : counters | std::views::reverse)
    {
        bitset = (bitset << 1) | (counter % 2);
    }

    return bitset;
}


/**
 * Apply a button sequence to counters and then divide each counter by 2.
 */
Counters ReduceCounters( Counters counters, const Buttons &sequence)
{
    // first apply the button sequence to the counters, this should
    // leave each counter an even value.
    for (const auto &button : sequence)
    {
        for (auto index = 0; index < counters.size(); ++index)
        {
            if (button & (1 << index))
            {
                --counters[index];
            }
        }
    }

    for (auto &counter: counters)
    {
        if (counter > 0) counter /= 2;
    }

    return counters;
}

Count Solve(
    const Counters &counters,
    const Buttons &buttons,
    ButtonSequenceCache &cache)
{
    // return 0 if all counters are 0
    if (AllZero(counters)) return 0;

    const auto lights = CountersToLights(counters);
    auto &buttonSequences = cache[lights];
    if (not buttonSequences)
    {
        buttonSequences = FindButtonSequences( CountersToLights( counters), buttons.begin(), buttons.end());
    }

    if (buttonSequences->empty()) return noResult;

    Count minimumCount = noResult;
    for ( const auto &sequence : *buttonSequences)
    {
        const auto reducedCounters = ReduceCounters( counters, sequence);
        if (AnyLessThanZero( reducedCounters)) continue;

        auto count = 2 * Solve( reducedCounters, buttons, cache) + sequence.size();
        if (count < minimumCount) minimumCount = count;
    }

    return minimumCount;

}

int main()
{
    Timer t;

    std::ifstream input{"input10.txt"};
    std::string line;

    const std::regex  machineLine{R"(\[([\.#]+)\]\s*((\([\d,]+\)\s+)+)\{([\d,]+)\})"};
    std::smatch m;
    size_t count = 0;

    while (getline(input, line) and regex_match(line, m, machineLine))
    {
        const auto buttons = StringToButtons( m[2]);
        const auto counters = StringToCounters( m[4]);
        ButtonSequenceCache cache;
        count += Solve( counters, buttons, cache);
    };

    std::cout << "count: " << count << '\n';
    return 0;
}
