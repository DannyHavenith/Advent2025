#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int absmod( int n, int m)
{
    // yes, I'm deliberately obfuscating this...
    return n < 0 ? m + n % m: n % m;
}

int main()
{
    std::ifstream input{"input1.txt"};
    std::string line;

    int pos = 50;
    std::size_t count = 0;

    while (getline(input, line))
    {
        assert( not line.empty());


        const auto oldPos = pos;

        const int sign = (line[0] == 'R')? 1 : -1;
        const auto steps = std::stoi( line.substr(1));

        const auto remainder = steps % 100;
        auto zeroPasses = steps / 100; // start with full rotation count

        pos += sign * remainder;
        if (remainder and oldPos and (pos <= 0 or pos >= 100))
        {
            ++zeroPasses;
        }

        pos = absmod(pos, 100);

        count += zeroPasses;

    }

    std::cout << "count is " << count << '\n';

    return 0;
}

