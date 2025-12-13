#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <set>
#include <vector>

#include "../timer.h"

using Shape = std::array<char,9>;
using ShapeVariants = std::set<Shape>;
using ShapeText = std::vector< std::string>;

// WARNING:
// You really don't want to read any further if you don't want to see a huge spoiler.

std::ostream &PrintShape( std::ostream &str, const Shape &s)
{
    const auto t = []( bool value) { return value?'#': '.';};

    str << t(s[0]) << t(s[1]) << t(s[2]) << '\n';
    str << t(s[3]) << t(s[4]) << t(s[5]) << '\n';
    str << t(s[6]) << t(s[7]) << t(s[8]) << '\n';

    return str;
}

Shape Flip( const Shape &i)
{
    return {
         i[2], i[1], i[0],
         i[5], i[4], i[3],
         i[8], i[7], i[6]
    };
}

Shape RotateRight( const Shape &i)
{
    return {
         i[6], i[3], i[0],
         i[7], i[4], i[1],
         i[8], i[5], i[2]
    };
}

// rather than parsing the input file, let's just
// use the 5 shapes in that file.
std::vector<Shape> shapes =
{
    { // 0
        1,0,0,
        1,1,0,
        1,1,1
    },
    { // 1
        1,1,1,
        0,1,0,
        1,1,1
    },
    { // 2
        0,0,1,
        1,1,1,
        1,1,1,
    },
    { // 3
        1,0,1,
        1,1,1,
        1,1,0
    },
    { // 4
        1,0,0,
        1,1,0,
        0,1,1
    },
    { // 5
        1,1,1,
        1,0,1,
        1,0,1
    }
};

ShapeVariants CreateRotations( Shape input)
{
    ShapeVariants result;
    result.insert( input);
    input = RotateRight( input);
    result.insert( input);
    input = RotateRight( input);
    result.insert( input);
    input = RotateRight( input);
    result.insert( input);
    return result;
}

ShapeVariants CreateShapeVariants( const Shape &inputShape)
{
    ShapeVariants result;
    std::ranges::set_union(
        CreateRotations( inputShape),
        CreateRotations( Flip( inputShape)),
        std::inserter( result, result.end()));
    return result;
}

int ShapeSquaresCount( const Shape &s)
{
    return std::count( s.begin(), s.end(), true);
}


std::size_t Solve( int width, int height, std::vector<int> &counts)
{
    std::cout << "Solve: " << width << 'x' << height << ':';
    for (const auto count : counts)
    {
        std::cout << ' ' << count;
    }
    std::cout << '\n';
    return 0;
}

std::size_t Solve( std::istream &input)
{
    static const std::regex region{"(\\d+)x(\\d+): (\\d+) (\\d+) (\\d+) (\\d+) (\\d+) (\\d+)"};
    std::string line;
    std::smatch m;
    std::size_t fitting = 0;

    while (getline( input, line))
    {
        if (regex_match( line, m, region))
        {
            auto width = stoi(m[1]);
            auto height = stoi(m[2]);
            std::vector<int> counts;
            int totalCount = 0;
            int squaresCount = 0;
            for (int i = 0; i <= 5; ++i)
            {
                auto shapeCount = stoi( m[3 + i]);
                counts.push_back( shapeCount);
                totalCount += shapeCount;
                auto shapeSize = ShapeSquaresCount( shapes[i]);
                squaresCount += shapeCount * shapeSize;
            }

            // get the trivial cases out of the way: first, if the number of
            // squares to be filled is higher than the number of squares in the
            // grid.
            if (width * height < squaresCount)
            {
                continue;
            }

            // second: If we can fit a 3x3 block as many times as
            // the total number of presents.
            if ( (width/3) * (height/3)  >= totalCount)
            {
                ++fitting;
                continue;
            }

            // solve the remaining cases the hard way.
            fitting += Solve( width, height, counts);
        }
    }

    return fitting;
}

int main()
{
    Timer t;

    std::ifstream input{"input12.txt"};

    std::cout << "matching: " << Solve( input) << '\n';
    return 0;
}
