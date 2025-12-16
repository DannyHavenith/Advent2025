#pragma once
#include <chrono>
#include <iostream>

class Timer
{
public:
    explicit Timer( std::ostream &out = std::cerr)
    :
    m_start{std::chrono::high_resolution_clock::now()},
    m_output{out}
    {}

    ~Timer()
    {
        m_output << "duration: " << std::chrono::duration_cast< std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_start) << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
    std::ostream &m_output;
};
