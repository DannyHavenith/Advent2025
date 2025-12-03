#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int MaxJoltage( std::string_view battery)
{
    assert( battery.size() >= 2);

    const auto firstDigit = std::max_element( battery.begin(), battery.end() - 1);
    const auto secondDigit = std::max_element( firstDigit + 1, battery.end());

    return 10 * (*firstDigit - '0') + (*secondDigit - '0');
}


int main()
{
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
