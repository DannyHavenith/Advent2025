#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "../timer.h"

int MaxJoltage( std::string_view bank)
{
    assert( bank.size() >= 2);

    const auto firstDigit = std::max_element( bank.begin(), bank.end() - 1);
    const auto secondDigit = std::max_element( firstDigit + 1, bank.end());

    return 10 * (*firstDigit - '0') + (*secondDigit - '0');
}


int main()
{
    Timer t;

    std::ifstream input{"input3.txt"};
    std::string line;

    int sum = 0;
    while (getline(input, line))
    {
        sum += MaxJoltage( line);
    }

    std::cout << "sum = " << sum << '\n';

    return 0;
}
