#include <algorithm>
#include <cassert>
#include <cstdint>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../timer.h"

namespace {
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
}

int main()
{
    Timer t;

    std::ifstream input{"input4.txt"};
    std::string nextLine;

    std::size_t count = 0;

    auto currentCount = Row();
    auto currentLRC = Row();
    auto currentRow = Row();

    // "C" stands for "Cell" which is 1 if a cell is occupied and 0 if not "LR"
    // stands for "Left Right count" which is the sum of the left cell and the
    // righ cell (0-2) "LRC" is the sum of "LR" and "C"
    //
    // This algorithm performs a line scan of the grid from top to bottom. The
    // line that is read is the 'next line' and what's called the 'current row'
    // has been determined in the previous iteration, if any. At the start of
    // each iteration:
    //    * currentCount holds for each cell the sum of all neighbours in the
    //      current and previous rows, but not yet of the next row
    //    * currentLRC holds the LRC count of the cells of the current row.
    //    * currentRow holds the occupation count of the current row.
    while (getline(input, nextLine))
    {
        auto nextRow = ToRow( nextLine);
        auto nextLR = CalculateLR( nextRow);
        auto nextLRC = nextRow + nextLR;

        AddTo( currentCount, nextLRC);
        if (not currentRow.empty())
        {
            for (auto i = 0U; i < currentCount.size();++i)
            {
                if (currentRow[i] and currentCount[i] < 4) ++count;
            }
        }

        // Move to the next line, current <- next
        currentCount = currentLRC + nextLR;
        currentLRC = nextLRC;
        currentRow = nextRow;
    }

    for (auto i = 0U; i < currentCount.size();++i)
    {
        if (currentRow[i] and currentCount[i] < 4) ++count;
    }

    std::cout << "count = " << count << '\n';

    return 0;
}
