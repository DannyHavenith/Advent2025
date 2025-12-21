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

namespace {
    /**
     * The solution here is not optimal. Probably, the fastest solution would be to
     * implement a linear equation system solver, but that seems a bit much for a
     * single puzzle. Alternatively we could use a solver library like Z3, but that
     * also feels like cheating until I know how Z3 implements its solving.
     */

    using Positions = std::set<int>;
    using Bitset = std::uint16_t; // max number of lights appears to be 10
    using Button = Bitset;
    using Buttons = std::vector<Button>;
    using Tokenizer = std::sregex_token_iterator;
    using Count = std::int64_t;
    using Counters = std::vector< Count>;

    // A pair consisting of the button count of a button sequence and the effects
    // that that sequence of button has on counters.
    using ButtonsEffectOnCounters = std::pair< int, Counters>;
    using ButtonEffects = std::vector<ButtonsEffectOnCounters>;
    using ButtonSequenceCache = std::map< Button, std::optional<ButtonEffects>>;

    static constexpr Count noResult = 1'000'000;

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

    bool AnyLessThanZero( const Counters &counters)
    {
        return std::ranges::any_of( counters, [](const auto number){return number < 0;});
    }

    bool AllZero( const Counters &counters)
    {
        return std::ranges::all_of( counters, [](const auto number){return number == 0;});
    }


    /**
     * Convert a sequence of buttons to the effect this sequence has on the counters.
     * Return a pair that holds:
     *     * the size of the original sequence
     *     * the effect on the counters
     */
    std::pair<int, Counters> SequenceToCounters( const Buttons &sequence)
    {
        Counters counters;
        for (auto button : sequence)
        {
            auto bit = 0U;
            while (button)
            {
                if (bit >= counters.size()) counters.resize( bit + 1);
                if (button % 2) ++counters[bit];
                button >>= 1;
                ++bit;
            }
        }

        return {sequence.size(), counters};
    }

    // let's do the brute force thing again.
    void FindButtonSequences(
        Bitset lights,
        Buttons::const_iterator begin, Buttons::const_iterator end,
        Buttons &buttonsSoFar,
        ButtonEffects &result)
    {
        if (begin == end)
        {
            if (not lights) result.push_back( SequenceToCounters( buttonsSoFar));
            return;
        }

        FindButtonSequences( lights, std::next( begin), end, buttonsSoFar, result);

        buttonsSoFar.push_back( *begin);
        FindButtonSequences( lights ^ *begin, std::next( begin), end, buttonsSoFar, result);
        buttonsSoFar.pop_back();
    }

    std::vector<std::pair< int, Counters>> FindButtonSequences( Bitset lights, Buttons::const_iterator begin, Buttons::const_iterator end)
    {
        ButtonEffects effects;
        Buttons buttonWorkSet;
        buttonWorkSet.reserve( std::distance(begin, end));
        FindButtonSequences( lights, begin, end, buttonWorkSet, effects);

        return effects;
    }


    // Create a bitmask that represents all counters that have an odd value
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
    Counters ReduceCounters( Counters counters, const Counters &reduction)
    {
        // first apply the button sequence to the counters, this should
        // leave each counter an even value.
        auto counterReduction = reduction.begin();
        for (auto &counter: counters)
        {
            if (counterReduction != reduction.end()) counter -= *counterReduction++;
            if (counter % 2) throw std::runtime_error( "couldn't reduce odd counters to even by applying a button sequence");
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
            buttonSequences = FindButtonSequences( lights, buttons.begin(), buttons.end());
        }

        if (buttonSequences->empty()) return noResult;

        Count minimumCount = noResult;
        for ( const auto &sequence : *buttonSequences)
        {
            const auto reducedCounters = ReduceCounters( counters, sequence.second);
            if (AnyLessThanZero( reducedCounters)) continue;

            auto count = 2 * Solve( reducedCounters, buttons, cache) + sequence.first;
            if (count < minimumCount) minimumCount = count;
        }

        return minimumCount;
    }
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
