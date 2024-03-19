// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>
#include <QMap>

#include <cstddef>
#include <deque>
#include <exception>
#include <mutex>

namespace mv {
    class Application;
}

namespace mv::util
{

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

    QString toString() const;
};

using MessageRecordPointers = std::deque<const MessageRecord*>;
using MessageRecords = std::deque<MessageRecord>;

/**
 * Global application logger
 *
 * Class for recording log messages
 *
 * @author Niels Dekker (original design) and Thomas Kroes (re-design and refactor)
 */
class Logger
{
public:
    static QMap<QtMsgType, QString> messageTypeNames;

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

    friend class mv::Application;
};

}

#define MV_LOG_EXCEPTION(stdException) qCritical().noquote() << ::mv::Logger::ExceptionToText(stdException)
