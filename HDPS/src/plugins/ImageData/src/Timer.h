#pragma once

#include <chrono>

#include <QString>

using SteadyClock = std::chrono::time_point<std::chrono::steady_clock>;
using MilliSeconds = std::chrono::microseconds;

// code inspired by: https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time

class Timer
{
public:
    Timer(const QString& event);
    ~Timer();

    std::int64_t elapsedTime() const;
    float elapsedTimeMilliseconds() const;

    void printElapsedTime(const QString& event, const bool& reset = false) const;
    void reset();

private:
    QString         _event;
    SteadyClock     _start;
};