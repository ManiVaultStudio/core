#pragma once

#include <chrono>

#include <QString>

using SteadyClock   = std::chrono::time_point<std::chrono::steady_clock>;
using MilliSeconds  = std::chrono::microseconds;

/**
 * Timer utility class
 *
 * Helper class for timing events
 * When an instance of this class goes out of scope, it prints the elapsed time (prefixed by the event name)
 *
 * Code inspired by: https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time
 *
 * @author Thomas Kroes
 */
class Timer
{
public: // Construction/destruction

    /**
     * Constructor
     * @param event Name of the event to measure (when empty, elapsed time is not printed when this object goes out of scope)
     */
    Timer(const QString& event = "");

    /** Destructor */
    ~Timer();

public: // Timer functions

    /**
     * Returns the elapsed time
     * @param start The start time of the time interval
     * @return Elapsed time
     */
    std::int64_t elapsedTime(const SteadyClock& start) const;

    /**
     * Returns the elapsed time
     * @param start The start time of the time interval
     * @return Elapsed time in milliseconds
     */
    float elapsedTimeMilliseconds(const SteadyClock& start) const;

    /**
     * Prints the elapsed time since the timer was created
     * @param event Name of the event
     * @param reset Whether to reset the start time
     */
    void printElapsedTime(const QString& event, const bool& reset = false);

    /** Resets the start time of the timer */
    void reset();

protected:

    /** Prints the elapsed time since the timer was created */
    void printTotalTime();

private:
    QString         _event;             /** Name of the event */
    SteadyClock     _start;             /** Global start time */
    SteadyClock     _eventStart;        /** Event start time */
};