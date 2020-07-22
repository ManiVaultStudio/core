#ifndef HDPS_LOGGER_H
#define HDPS_LOGGER_H

#include <QString>

// Standard C++ header files:
#include <cstddef>
#include <deque>
#include <exception>

namespace hdps
{

struct MessageRecord
{
    std::size_t number;
    QtMsgType type;
    int version;
    int line;
    const char *file;
    const char *function;
    const char *category;
    QString message;
};


class Logger
{
public:
    static QString MsgTypeToString(QtMsgType);
    static void Initialize();
    static QString GetFilePathName();
    static QString ExceptionToText(const std::exception& stdException);
    static void GetMessageRecords(std::deque<const MessageRecord*>&);
};

}   //namespace hdps

#define HDPS_LOG_EXCEPTION(stdException) qCritical().noquote() << \
  ::hdps::Logger::ExceptionToText(stdException)


#endif // HDPS_LOGGER_H
