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
within 5ms (most within 1ms), but days 8 and 9 currently require more time, up
to 340ms for day 8.

Timings
-------
These measurements were made on an AMD Ryzen 9 3900X processor, after repeatedly
running the executables to warm up disk caches.

| Day | Part | Duration |
|-----|------|----------|
| 1   | a    | 434us    |
| 1   | b    | 471us    |
| 2   | a    | 246us    |
| 2   | b    | 279us    |
| 3   | a    | 149us    |
| 3   | b    | 199us    |
| 4   | a    | 231us    |
| 4   | b    | 5.1ms    |
| 5   | a    | 652us    |
| 5   | b    | 484us    |
| 6   | a    | 2.1ms    |
| 6   | b    | 3.7ms    |
| 7   | a    | 727us    |
| 7   | b    | 999us    |
| 8   | a    | 340.6ms  |
| 8   | b    | 348.7ms  |
| 9   | a    | 24.4ms   |
| 9   | b    | 40.7ms   |
| 10  | a    | 5.2ms    |
| 11  | a    | 3.3ms    |
| 11  | b    | 3.2ms    |
| 12  | a    | 2.2ms    |
