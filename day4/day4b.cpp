#include <algorithm>
#include <cassert>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Cell = uint8_t;
using Row = std::vector<Cell>;
using Grid = std::vector<Row>;


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

Grid ReadGrid( std::istream &textInput)
{
    Grid result;
    std::string line;
    while (getline(textInput, line))
    {
        result.push_back( ToRow( line));
    }
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
    auto grid = ReadGrid( input);
    assert( not grid.empty());

    std::size_t removedTotal = 0;


    int removedThisIteration = 0;
    do
    {
        auto currentCount = Row();
        auto currentLRC = Row();
        Row *currentRow = nullptr;

        // "C" stands for "Cell" which is 1 if a cell is occupied and 0 if not "LR"
        // stands for "Left Right count" which is the sum of the left cell and the
        // righ cell (0-2) "LRC" is the sum of "LR" and "C"
        //
        // This algorithm performs a line scan of the grid from top to bottom. The
        // line that is visited is the 'next line' and what's called the 'current row'
        // has been determined in the previous iteration, if any. At the start of
        // each iteration:
        //    * currentCount holds for each cell the sum of all neighbours in the
        //      current and previous rows, but not yet of the next row
        //    * currentLRC holds the LRC count of the cells of the current row.
        //    * currentRow holds the occupation count of the current row.
        removedThisIteration = 0;
        for (auto &nextRow : grid)
        {
            auto nextLR = CalculateLR( nextRow);
            auto nextLRC = nextRow + nextLR;

            AddTo( currentCount, nextLRC);
            if (currentRow)
            {
                for (auto i = 0; i < currentCount.size();++i)
                {
                    if ((*currentRow)[i] and currentCount[i] < 4)
                    {
                        ++removedThisIteration;
                        (*currentRow)[i] = 0; // remove
                    }
                }
            }

            // Move to the next line, current <- next
            currentCount = currentLRC + nextLR;
            currentLRC = nextLRC;
            currentRow = &nextRow;
        }

        for (auto i = 0; i < currentCount.size();++i)
        {
            if ((*currentRow)[i] and currentCount[i] < 4)
            {
                ++removedThisIteration;
                (*currentRow)[i] = 0;
            }
        }

        removedTotal += removedThisIteration;
    }
    while (removedThisIteration);

    std::cout << "count = " << removedTotal << '\n';

    return 0;
}
