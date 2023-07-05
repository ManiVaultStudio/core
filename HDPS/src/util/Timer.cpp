// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Timer.h"

#include <QDebug>

Timer::Timer(const QString& event /*= ""*/) :
    _event(event),
    _start(),
    _eventStart()
{
    _start = std::chrono::steady_clock::now();

    reset();
}

Timer::~Timer()
{
    if (!_event.isEmpty())
        printTotalTime();
}

std::int64_t Timer::elapsedTime(const SteadyClock& start) const
{
    return std::chrono::duration_cast<MilliSeconds>(std::chrono::steady_clock::now() - start).count();
}

float Timer::elapsedTimeMilliseconds(const SteadyClock& start) const
{
    return static_cast<float>(elapsedTime(start)) / 1000.f;
}

void Timer::printElapsedTime(const QString& event, const bool& reset /*= false*/)
{
    qDebug() << event << "took" << QString::number(elapsedTimeMilliseconds(_eventStart), 'f', 2) << "ms";

    if (reset)
        this->reset();
}

void Timer::reset()
{
    _eventStart = std::chrono::steady_clock::now();
}

void Timer::printTotalTime()
{
    qDebug() << _event << "took" << QString::number(elapsedTimeMilliseconds(_start), 'f', 2) << "ms";
}