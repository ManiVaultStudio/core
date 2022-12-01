#pragma once

#include <QString>
#include <QMap>

// Standard C++ header files:
#include <cstddef>
#include <deque>
#include <exception>
#include <mutex>

namespace hdps
{
    class Application;

    struct MessageRecord
    {
        std::size_t number;
        QtMsgType type;
        int version;
        int line;
        const char* file;
        const char* function;
        const char* category;
        QString message;
    };

    using MessageRecordPointers = std::deque<const MessageRecord*>;
    using MessageRecords = std::deque<MessageRecord>;

    class Logger
    {
    public:
        static QMap<QtMsgType, QString> messageTypeNames;

    protected:
        Logger() {};

    public:
        static QString getMessageTypeName(QtMsgType);
        
        static QString GetFilePathName();
        static QString ExceptionToText(const std::exception& stdException);
        
        void initialize();
        MessageRecords& getMessageRecords();
        void updateMessageRecords(MessageRecordPointers& messageRecordPointers);
        std::mutex& getMutex();

    private:
        MessageRecords  _messageRecords;    /** Messages recorded since the instantiation of the logger */
        std::mutex      _mutex;             /** Prevent race conditions */

        friend class Application;
    };

}

#define HDPS_LOG_EXCEPTION(stdException) qCritical().noquote() << \
  ::hdps::Logger::ExceptionToText(stdException)