#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>

#include "../timer.h"

namespace {

    using Number = std::int64_t;
    using Accumulator = std::optional<Number>;
    using Accumulators = std::vector<Accumulator>;
}

int main()
{
    Timer t;

    std::ifstream input{"input6.txt"};
    Accumulators accumulators;

    std::string line;
    getline( input, line);
    std::transform(
        line.begin(), line.end(),
        std::back_inserter( accumulators),
        []( char maybeDigit){ return maybeDigit == ' ' ? std::nullopt : Accumulator{ maybeDigit - '0'};}
    );

    const std::regex numbersLine{"([\\d\\s])+"};
    while (getline(input, line) and regex_match( line, numbersLine))
    {
        if (line.size() > accumulators.size()) accumulators.resize( line.size());

        assert( line.size() == accumulators.size());
        auto currentCharacter = line.begin();
        for ( auto &ac : accumulators)
        {
            if (*currentCharacter != ' ')
            {
                const auto value = *currentCharacter - '0';
                ac = ac ? *ac * 10 + value : value;
            }
            ++currentCharacter;
        }
    }

    Number sum = 0;


    static const std::regex opExpression{R"(([\+\*]))"};
    auto currentAccumulator = accumulators.begin();
    using Tok = std::sregex_token_iterator;
    for ( auto currentOp = Tok{line.begin(), line.end(), opExpression, 1}; currentOp != Tok{}; ++currentOp, ++currentAccumulator)
    {
        while (not currentAccumulator->has_value()) ++currentAccumulator;
        assert( currentAccumulator != accumulators.end());
        std::function<Number (Number, Number)> op;

        Number subSum = 0;
        if (*currentOp == '+')
        {
            op = std::plus<Number>{};
            subSum = 0;
        }
        else
        {
            op = std::multiplies<Number>{};
            subSum = 1;
        }

        while( currentAccumulator != accumulators.end() and currentAccumulator->has_value())
        {
            subSum = op( subSum, **currentAccumulator++);
        }
        sum += subSum;
    }

    std::cout << "sum = " << sum << '\n';

    return 0;
}
