#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "../timer.h"

namespace {
    // All (horizontal) positions where a beam is traveling.
    using Positions = std::set<int>;

    Positions FindAll( char needle, std::string_view haystack)
    {
        Positions result;
        for (auto i = haystack.begin(); i != haystack.end(); ++i)
        {
            if (*i == needle) result.insert( std::distance( haystack.begin(), i));
        }
        return result;
    }
}

int main()
{
    Timer t;

    std::ifstream input{"input7.txt"};
    std::string line;
    size_t splits = 0;

    getline( input, line);
    auto beams = FindAll( 'S', line);

    while (getline(input, line))
    {
        const auto splitters = FindAll( '^', line);

        std::vector<int> hits;
        std::ranges::set_intersection(
            beams,
            splitters,
            std::back_inserter( hits));

        splits += hits.size();

        for ( const auto pos: hits)
        {
            beams.erase( pos);
            beams.insert( pos - 1);
            beams.insert( pos + 1);
        }
    }

    std::cout << "splits: " << splits << '\n';

    return 0;
}
