#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "../timer.h"

namespace {
    using Number = std::int64_t;

    struct Accumulator {
        Accumulator( Number initialProduct, Number initialSum)
        :product{initialProduct}, sum{initialSum}
        {}

        Number product;
        Number sum;
    };

    using Accumulators = std::vector<Accumulator>;
    using Tokenizer = std::sregex_token_iterator;
}

int main()
{
    Timer t;

    std::ifstream input{"input6.txt"};
    std::string line;
    Accumulators accumulators;

    const std::regex matchNumber{"(\\d+)"};

    // Read the first line and initialize an Accumulator vector of the right size.
    getline( input, line);
    for (
        auto i = Tokenizer{ line.begin(), line.end(), matchNumber, 1};
        i != Tokenizer{};
        ++i)
    {
        auto number = stoi( *i);
        accumulators.emplace_back( number, number);
    }

    // Scan the lines and keep a running sum and product for the columns.
    while (getline(input, line))
    {
        bool parsedANumber = false;
        auto currentAccumulator = accumulators.begin();

        for ( auto i = Tokenizer{ line.begin(), line.end(), matchNumber, 1} ;
              i != Tokenizer{};
              ++i, ++currentAccumulator)
        {
            assert( currentAccumulator != accumulators.end());

            parsedANumber = true;
            auto number = stoi( *i);
            *currentAccumulator = {currentAccumulator->product * number, currentAccumulator->sum + number};
        }
        if (not parsedANumber) break; // no numbers, so this is the line with operators.
    }

    // For each column now either take the sum or the product and add it to the
    // total.
    const std::regex matchOperator{"([\\+\\*])"};
    auto currentAccumulator = accumulators.begin();
    Number sum = 0;
    for (
        auto currentOp = Tokenizer{line.begin(), line.end(), matchOperator, 1};
        currentOp != Tokenizer{};
        ++currentOp, ++currentAccumulator)
    {
        sum += *currentOp == '+' ?
                    currentAccumulator->sum :
                    currentAccumulator->product;
    }

    std::cout << "sum = " << sum << '\n';

    return 0;
}
