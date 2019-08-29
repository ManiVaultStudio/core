#pragma once

#include <QString>
#include <deque>
#include <exception>

namespace hdps
{

struct MessageRecord
{
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
    static void Initialize();
    static QString GetFilePathName();
    static QString ExceptionToText(const std::exception& stdException);
    static void GetMessageRecords(std::deque<const MessageRecord*>&);
};

}   //namespace hdps

#define HDPS_LOG_EXCEPTION(stdException) qCritical().noquote() << \
  ::hdps::Logger::ExceptionToText(stdException)

