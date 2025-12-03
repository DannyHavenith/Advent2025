#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    std::ifstream input{"input1.txt"};
    std::string line;

    int pos = 50;
    std::size_t count = 0;

    while (getline(input, line))
    {
        assert( not line.empty());

        const int sign = (line[0] == 'R')? 1 : -1;
        const int steps = std::stoi( line.substr(1));

        pos = (pos + sign * steps) % 100;

        if (not pos) ++count;
    }

    std::cout << "count is " << count << '\n';

    return 0;
}
