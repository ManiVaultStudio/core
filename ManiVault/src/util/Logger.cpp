// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// Its own header file:
#include "Logger.h"

#include "Application.h"

#include "util/StandardPaths.h"

// Qt header files:
#include <QByteArray>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QString>
#include <QtGlobal> // For qInstallMessageHandler

// Standard C++ header files:
#include <atomic>
#include <cassert>
#include <cstdint> // For uint8_t.
#include <deque>
#include <fstream>
#include <string>
#include <typeinfo>

using namespace mv;
using namespace mv::util;

namespace
{

    constexpr auto separator = '\t';


    auto ConvertToQString(const char* const utf8Chars)
    {
        return QString::fromUtf8(utf8Chars);
    }

    auto ConvertToQString(const wchar_t* const wideChars)
    {
        return QString::fromWCharArray(wideChars);
    }


    auto CreateLogFilePathName()
    {
        const auto currentDateTime = QDateTime::currentDateTime();
        const QDir logDir = StandardPaths::getLogsDirectory();
        const QFileInfo applicationFileInfo(QCoreApplication::applicationFilePath());

        return logDir.filePath(applicationFileInfo.completeBaseName()
            + QLatin1String("_")
            + currentDateTime.toString(QLatin1String("yyyyMMdd_hh-mm-ss-zzz"))
            + QLatin1String(".log"));
    }


#ifdef _WIN32
    std::wstring ConvertToStdBasicString(const QString& str)
    {
        const void* const utf16 = str.utf16();
        return static_cast<const wchar_t*>(utf16);
    }
#else
    std::string ConvertToStdBasicString(const QString& str)
    {
        return str.toUtf8().constData();
    }
#endif


    const auto& GetFilePathName()
    {
        const static auto fileNamePath = ConvertToStdBasicString(CreateLogFilePathName());

        return fileNamePath;
    }


    const char* MakeNullPrintable(const char* const arg, const char* const nullText) noexcept
    {
        return (arg == nullptr) ? nullText : arg;
    }


    QtMessageHandler GetPreviousMessageHandler(const QtMessageHandler previousMessageHandler = nullptr) noexcept
    {
        const static auto result = previousMessageHandler;

        return result;
    }


    void ReplaceUnprintableAsciiCharsBySpaces(QByteArray& byteArray)
    {
        for (auto& byte : byteArray)
        {
            // If the byte is an unprintable ASCII char...
            if ((static_cast<std::uint8_t>(byte) < std::uint8_t{ ' ' }) || (byte == SCHAR_MAX))
            {
                // ...replace its value by a space.
                byte = ' ';
            }
        }
    }


    class LogFile
    {
    private:
        std::ofstream m_outputStream;
    public:
        LogFile(const LogFile&) = delete;
        LogFile(LogFile&&) = delete;
        LogFile& operator=(const LogFile&) = delete;
        LogFile& operator=(LogFile&&) = delete;

        LogFile() noexcept(false)
            :
            m_outputStream(GetFilePathName())
        {
            // "The Unicode Standard permits the BOM (byte order mark) in UTF-8, but
            // does not require or recommend its use"
            // https://en.wikipedia.org/wiki/Byte_order_mark
            const char bom[] = { '\xEF', '\xBB', '\xBF' };
            m_outputStream.write(bom, sizeof(bom));

            m_outputStream
                << "number"
                << separator << "category"
                << separator << "type"
                << separator << "version"
                << separator << "file"
                << separator << "line"
                << separator << "function"
                << separator << "message"
                << std::endl;
        }

        std::ofstream& GetOutputStream() noexcept
        {
            return m_outputStream;
        }

        explicit operator bool() const
        {
            return m_outputStream ? true : false;
        }

        ~LogFile()
        {
            // Uninstall custom message handlers to avoid crashed when unloading plugins.
            // We observed that when environment variable QT_DEBUG_PLUGINS is set,
            // "QLibraryPrivate::unload succeeded" messages may still be passed to the
            // message handler when the log file is already destructed.
            qInstallMessageHandler(nullptr);
        }
    };

    struct SetAtomicBoolFalseAtScopeExit
    {
    private:
        std::atomic_bool& _atomicBool;
    public:
        SetAtomicBoolFalseAtScopeExit(const SetAtomicBoolFalseAtScopeExit&) = delete;
        SetAtomicBoolFalseAtScopeExit(SetAtomicBoolFalseAtScopeExit&&) = delete;
        SetAtomicBoolFalseAtScopeExit& operator=(const SetAtomicBoolFalseAtScopeExit&) = delete;
        SetAtomicBoolFalseAtScopeExit& operator=(SetAtomicBoolFalseAtScopeExit&&) = delete;

        explicit SetAtomicBoolFalseAtScopeExit(std::atomic_bool& atomicBool) noexcept
            :
            _atomicBool{ atomicBool }
        {
        }

        ~SetAtomicBoolFalseAtScopeExit()
        {
            _atomicBool = false;
        }
    };


    void MessageHandler(
        const QtMsgType type,
        const QMessageLogContext& context,
        const QString& message)
    {
        // Avoid recursion.
        static std::atomic_bool isExecutingMessageHandler{ false };

        if (!isExecutingMessageHandler)
        {
            isExecutingMessageHandler = true;

            const SetAtomicBoolFalseAtScopeExit setAtomicBoolFalseAtScopeExit{ isExecutingMessageHandler };

            static LogFile logFile;
            const auto previousMessageHandler = GetPreviousMessageHandler();

            auto utf8Message = message.toUtf8();
            ReplaceUnprintableAsciiCharsBySpaces(utf8Message);
            const char* const utf8MessageData = utf8Message.constData();

            if (previousMessageHandler != nullptr)
            {
                previousMessageHandler(type, context, message);
            }

            // Get reference to application-wide logger
            auto& logger = Application::getLogger();

            // Lock the mutex from here, to grant exclusive access to 
            // MessageRecords and log file.
            const std::lock_guard<std::mutex> guard(logger.getMutex());

            auto& messageRecords = Application::getLogger().getMessageRecords();

            const auto messageNumber = messageRecords.size() + 1;

            for (const auto& messageSegment : message.split("\n")) {
                messageRecords.push_back(
                    {
                        messageNumber,
                        type,
                        context.version,
                        context.line,
                        context.file,
                        context.function,
                        context.category,
                        messageSegment
                    });

                if (logFile)
                {
                    logFile.GetOutputStream()
                        << messageNumber
                        << separator << MakeNullPrintable(context.category, "<category>")
                        << separator << mv::util::Logger::getMessageTypeName(type).toStdString()
                        << separator << context.version
                        << separator << MakeNullPrintable(context.file, "<file>")
                        << separator << context.line
                        << separator << MakeNullPrintable(context.function, "<function>")
                        << separator << '"' << utf8MessageData << '"'
                        << std::endl;
                }
            }
        }
    }

}   // namespace

namespace mv::util {

Logger::~Logger()
{
    qInstallMessageHandler(0);
}

QMap<QtMsgType, QString> Logger::messageTypeNames = {
    { QtDebugMsg, "Debug" },
    { QtWarningMsg, "Warning" },
    { QtCriticalMsg, "Critical" },
    { QtFatalMsg, "Fatal" },
    { QtInfoMsg, "Info" }
};

QString Logger::getMessageTypeName(const QtMsgType msgType)
{
    return Logger::messageTypeNames[msgType];
}

void Logger::initialize()
{
    (void)GetPreviousMessageHandler(qInstallMessageHandler(&MessageHandler));
}


QString Logger::GetFilePathName()
{
    return ConvertToQString(::GetFilePathName().c_str());
}

QString Logger::ExceptionToText(const std::exception& stdException)
{
    return QObject::tr("Caught exception: \"%1\". Type: %2")
        .arg(stdException.what())
        .arg(typeid(stdException).name());
}

MessageRecords& Logger::getMessageRecords()
{
    return _messageRecords;
}

void Logger::updateMessageRecords(MessageRecordPointers& messageRecordPointers)
{
    // Lock the mutex from here.
    const std::lock_guard<std::mutex> guard(getMutex());

    const auto previousNumberOfMessages = messageRecordPointers.size();
    const auto numberOfMessages = _messageRecords.size();

    if (previousNumberOfMessages <= numberOfMessages)
    {
        for (auto i = 0; i < previousNumberOfMessages; ++i)
        {
            assert(messageRecordPointers[i] == &(_messageRecords[i]));
        }
    }
    else
    {
        assert(!"The number of messages should not be less than the previous time!");
    }

    messageRecordPointers.resize(numberOfMessages);

    for (auto i = previousNumberOfMessages; i < numberOfMessages; ++i)
        messageRecordPointers[i] = &(_messageRecords[i]);
}

std::mutex& Logger::getMutex()
{
    return _mutex;
}

QString MessageRecord::toString() const
{
    QStringList items;

    items << QString::number(number);
    items << Logger::getMessageTypeName(type);
    items << QString(file);
    items << QString::number(line);
    items << QString(function);
    items << QString(category);
    items << message;

    return items.join("\t");
}

}
