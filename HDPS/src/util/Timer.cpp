#include "Timer.h"

#include <QDebug>

Timer::Timer(const QString& event) :
    _event(event),
    _start()
{
    reset();
}

Timer::~Timer()
{
    printElapsedTime(_event);
}

std::int64_t Timer::elapsedTime() const
{
    return std::chrono::duration_cast<MilliSeconds>(std::chrono::steady_clock::now() - _start).count();
}

float Timer::elapsedTimeMilliseconds() const
{
    return static_cast<float>(elapsedTime()) / 1000.f;
}

void Timer::printElapsedTime(const QString& event, const bool& reset /*= false*/) const
{
    qDebug() << event << "took" << QString::number(elapsedTimeMilliseconds(), 'f', 2) << "ms";
}

void Timer::reset()
{
    _start = std::chrono::steady_clock::now();
}