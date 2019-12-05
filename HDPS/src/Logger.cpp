// Its own header file:
#include "Logger.h"

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
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint> // For uint8_t.
#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <typeinfo>


namespace
{

constexpr auto separator = '\t';


std::string MsgTypeToAsciiString(const QtMsgType msgType)
{
    switch (msgType)
    {
    case QtDebugMsg: return "debug";
    case QtWarningMsg: return "warning";
    case QtCriticalMsg: return "critical";
    case QtFatalMsg: return "fatal";
    case QtInfoMsg: return "info";
    }
    return std::to_string(msgType);
}


auto ConvertToQString(const char* const utf8Chars)
{
    return QString::fromUtf8(utf8Chars);
}

auto ConvertToQString(const wchar_t* const wideChars)
{
    return QString::fromWCharArray(wideChars);
}


auto GetLogDirectoryPathName()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}


auto CreateLogFilePathName()
{
    const auto currentDateTime = QDateTime::currentDateTime();
    const QDir logDir = GetLogDirectoryPathName();
    const QFileInfo applicationFileInfo = QCoreApplication::applicationFilePath();

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


auto& GetMessageRecords()
{
    static std::deque<hdps::MessageRecord> result;
    return result;
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


static auto& GetMutex() noexcept
{
    static std::mutex result;
    return result;
}


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
    const QMessageLogContext &context,
    const QString &message)
{
    // Avoid recursion.  // TODO Just testing C++17 support
    if (static std::atomic_bool isExecutingMessageHandler{ false }; !isExecutingMessageHandler)
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

        // Lock the mutex from here, to grant exclusive access to 
        // MessageRecords and log file.
        const std::lock_guard<std::mutex> guard(GetMutex());

        auto& messageRecords = GetMessageRecords();
        const auto messageNumber = messageRecords.size() + 1;

        messageRecords.push_back(
            {
                messageNumber,
                type,
                context.version,
                context.line,
                context.file,
                context.function,
                context.category,
                message
            });

        if (logFile)
        {
            logFile.GetOutputStream()
                << messageNumber
                << separator << MakeNullPrintable(context.category, "<category>")
                << separator << MsgTypeToAsciiString(type)
                << separator << context.version
                << separator << MakeNullPrintable(context.file, "<file>")
                << separator << context.line
                << separator << MakeNullPrintable(context.function, "<function>")
                << separator << '"' << utf8MessageData << '"'
                << std::endl;
        }
    }
}

}   // namespace


QString hdps::Logger::MsgTypeToString(const QtMsgType msgType)
{
    return QString::fromLatin1(MsgTypeToAsciiString(msgType).c_str());
}

void hdps::Logger::Initialize()
{
    QDir{}.mkpath(GetLogDirectoryPathName());
    (void)GetPreviousMessageHandler(qInstallMessageHandler(&MessageHandler));
}


QString hdps::Logger::GetFilePathName()
{
    return ConvertToQString(::GetFilePathName().c_str());
}


QString hdps::Logger::ExceptionToText(const std::exception& stdException)
{
    return QObject::tr("Caught exception: \"%1\". Type: %2")
        .arg(stdException.what())
        .arg(typeid(stdException).name());
}


void hdps::Logger::GetMessageRecords(std::deque<const MessageRecord*>& messageRecordPointers)
{
    // Lock the mutex from here.
    const std::lock_guard<std::mutex> guard(GetMutex());

    const auto& messageRecords = ::GetMessageRecords();
    const auto previousNumberOfMessages = messageRecordPointers.size();
    const auto numberOfMessages = messageRecords.size();

    if (previousNumberOfMessages <= numberOfMessages)
    {
#if defined(__clang__) || ! defined(__GNUC__)
        // No Gcc compile because of https://travis-ci.com/bldrvnlw/conan-hdps-core/jobs/263538709
/*
error: no matching function for call to ‘for_each_n(std::deque<const hdps::MessageRecord*>::const_iterator, const long unsigned int&, hdps::Logger::GetMessageRecords(std::deque<const hdps::MessageRecord*>&)::<lambda(auto:1)>)’
*/
        // TODO Just testing C++17 support
        std::size_t i{};
        std::for_each_n(messageRecordPointers.cbegin(), previousNumberOfMessages, [&i, &messageRecords](const auto messageRecordPointer)
        {
            assert(messageRecordPointer == &(messageRecords[i]));
            ++i;
        });
#endif
    }
    else
    {
        assert(!"The number of messages should not be less than the previous time!");
    }

    messageRecordPointers.resize(numberOfMessages);

    for (auto i = previousNumberOfMessages; i < numberOfMessages; ++i)
    {
        messageRecordPointers[i] = &(messageRecords[i]);
    }
}
