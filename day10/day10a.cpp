#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "../timer.h"

namespace {
    using Positions = std::set<int>;
    using Bitset = std::uint16_t; // max number of lights appears to be 10
    using Button = Bitset;
    using Buttons = std::vector<Button>;
    using Tokenizer = std::sregex_token_iterator;

    struct Machine
    {
        Bitset target;
        Buttons buttons;
    };

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

    // let's do the brute force thing again.
    std::size_t Solve( Bitset lights, Buttons::const_iterator begin, Buttons::const_iterator end, std::size_t count = 0)
    {
        if (not lights) return count;
        if (begin == end) return 1'000'000;

        auto solution1 = Solve( lights, std::next( begin), end, count);
        auto solution2 = Solve( lights ^ *begin, std::next( begin), end, count + 1);

        return std::min(solution1, solution2);
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
        const auto lights = StringToLights(m[1]);
        const auto buttons = StringToButtons( m[2]);
        count += Solve(
            lights,
            buttons.begin(), buttons.end());
    }

    std::cout << "count: " << count << '\n';

    return 0;
}
