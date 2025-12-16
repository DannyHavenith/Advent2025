My Advent of Code 2025 solutions
===============================

This repository contains my solutions for the Advent of Code 2025 programming
challenges. My personal goal is to write solutions with the lowest big-O time
complexity possible for each problem, with a secondary goal of writing the least
amount of code to achieve that.

The performance goal is mainly reached by avoiding brute-force solutions even if
the problems posed have small input sizes.

I might abandon any of these goals if I get desperate and so far this has
happened for day 9, where it took me too much time to implement a row-sweeping
algorithm.

All solutions are written in C++. The solutions for the first few days run well
within 5ms (most within 1ms), but days 8, 9 and 10 currently require more time,
up to 1.2s for day 10b.

Timings
-------
These measurements were made on an AMD Ryzen 9 3900X processor, with compiler
optimizations to the max,after repeatedly running the executables to warm up
disk caches. The timings do include all file reads necessary to read the input
data.

| Day | Part | Duration |
|-----|------|----------|
| 1   | a    | 419us    |
| 1   | b    | 467us    |
| 2   | a    | 248us    |
| 2   | b    | 252us    |
| 3   | a    | 147us    |
| 3   | b    | 210us    |
| 4   | a    | 134us    |
| 4   | b    | 4.4ms    |
| 5   | a    | 693us    |
| 5   | b    | 502us    |
| 6   | a    | 2.2ms    |
| 6   | b    | 5.8ms    |
| 7   | a    | 835us    |
| 7   | b    | 1.1ms    |
| 8   | a    | 33.3ms   |
| 8   | b    | 40.9ms   |
| 9   | a    | 44.1ms   |
| 9   | b    | 66.5ms   |
| 10  | a    | 2.6ms    |
| 10  | b    | 1.23s (!)|
| 11  | a    | 3.2ms    |
| 11  | b    | 3.3ms    |
| 12  | a    | 1.2ms    |
