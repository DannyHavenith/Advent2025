#include <algorithm>
#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Cell = uint8_t;
using Row = std::vector<Cell>;

Row ToRow( std::string_view line)
{
    Row result(line.size());
    std::transform(
        line.begin(), line.end(),
        result.begin(),
        [](char cell) -> Cell { return cell == '@';}
    );
    return result;
}

Row &AddTo( Row &target, const Row &source)
{
    if (target.empty())
    {
        target = source;
    }
    else
    {
        std::transform(
            target.begin(), target.end(),
            source.begin(),
            target.begin(),
            std::plus{}
        );
    }
    return target;
}

Row operator+( Row left, const Row &right)
{
    AddTo( left, right);
    return left;
}

/// Calculate the number of direct left and right neighbours of each cell.
Row CalculateLR( const Row &row)
{
    Row result( row.begin() + 1, row.end());
    result.push_back(0);

    std::transform(
        row.begin(), row.end() - 1,
        result.begin() + 1,
        result.begin() + 1,
        std::plus{}
    );

    return result;
}

int main()
{
    // "C" stands for "Cell" which is 1 if a cell is occupied and 0 if not
    // "LR" stands for "Left Right count" which is the sum of the left cell and the righ cell (0-2)
    // "LRC" is the sum of "LR" and "C"

    std::ifstream input{"input4.txt"};
    std::string nextLine;

    std::size_t count = 0;

    const auto rowSize = nextLine.size();

    auto currentCount = Row();
    auto currentLRC = Row();
    auto currentRow = Row();

    while (getline(input, nextLine))
    {
        auto nextRow = ToRow( nextLine);
        auto nextLR = CalculateLR( nextRow);
        auto nextLRC = nextRow + nextLR;

        AddTo( currentCount, nextLRC);
        if (not currentRow.empty())
        {
            for (auto i = 0; i < currentCount.size();++i)
            {
                if (currentRow[i] and currentCount[i] < 4) ++count;
            }
        }

        // move to the next line current <- next
        currentCount = currentLRC + nextLR;
        currentLRC = nextLRC;
        currentRow = nextRow;
    }

    for (auto i = 0; i < currentCount.size();++i)
    {
        if (currentRow[i] and currentCount[i] < 4) ++count;
    }

    std::cout << "count = " << count << '\n';

    return 0;
}
