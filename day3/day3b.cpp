#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "../timer.h"

using Number = std::uint64_t;

template<int digitCount = 12>
Number MaxJoltage( std::string_view bank)
{
    assert (bank.size() >= digitCount);

    auto lastposition = bank.begin();
    Number result = 0;
    for (auto n = 0; n < digitCount; ++n)
    {
        lastposition = std::max_element( lastposition, bank.end() - digitCount + n + 1);
        result = 10 * result + (*lastposition - '0');
        ++lastposition;
    }

    return result;
}


int main()
{
    Timer t;

    std::ifstream input{"input3.txt"};
    std::string line;

    Number sum = 0;
    while (getline(input, line))
    {
        sum += MaxJoltage( line);
    }

    std::cout << "sum = " << sum << '\n';

    return 0;
}
