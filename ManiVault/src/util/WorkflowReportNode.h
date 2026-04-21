// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMessage.h"

#include <QString>
#include <QDateTime>
#include <QMutex>

namespace mv::util
{

class CORE_EXPORT WorkflowReportNode
{
public:
    using Ptr = std::shared_ptr<WorkflowReportNode>;

    explicit WorkflowReportNode(const QString& name)
        : _name(name)
    {
    }

    Ptr createChild(const QString& name)
    {
        QMutexLocker lock(&_mutex);

        auto child = std::make_shared<WorkflowReportNode>(name);
        _children.push_back(child);
        return child;
    }

    void addMessage(WorkflowMessageLevel level,
        const QString& source,
        const QString& text)
    {
        QMutexLocker lock(&_mutex);

        _messages.push_back(WorkflowMessage{
            level,
            source,
            text,
            _name,
            QDateTime::currentDateTime()
            });
    }

    QString getName() const
    {
        QMutexLocker lock(&_mutex);
        return _name;
    }

    QVector<WorkflowMessage> getMessages() const
    {
        QMutexLocker lock(&_mutex);
        return _messages;
    }

    QVector<Ptr> getChildren() const
    {
        QMutexLocker lock(&_mutex);
        return _children;
    }

    bool hasErrorsRecursive() const
    {
        QVector<Ptr> childrenCopy;

        {
            QMutexLocker lock(&_mutex);

            for (const auto& message : _messages) {
                if (message._level == WorkflowMessageLevel::Error)
                    return true;
            }

            childrenCopy = _children;
        }

        for (const auto& child : childrenCopy) {
            if (child->hasErrorsRecursive())
                return true;
        }

        return false;
    }

    int getWarningCountRecursive() const
    {
        int result = 0;
        QVector<Ptr> childrenCopy;

        {
            QMutexLocker lock(&_mutex);

            for (const auto& message : _messages) {
                if (message._level == WorkflowMessageLevel::Warning)
                    ++result;
            }

            childrenCopy = _children;
        }

        for (const auto& child : childrenCopy)
            result += child->getWarningCountRecursive();

        return result;
    }

    int getErrorCountRecursive() const
    {
        int result = 0;
        QVector<Ptr> childrenCopy;

        {
            QMutexLocker lock(&_mutex);

            for (const auto& message : _messages) {
                if (message._level == WorkflowMessageLevel::Error)
                    ++result;
            }

            childrenCopy = _children;
        }

        for (const auto& child : childrenCopy)
            result += child->getErrorCountRecursive();

        return result;
    }

private:
    QString _name;
    mutable QMutex _mutex;
    QVector<WorkflowMessage> _messages;
    QVector<Ptr> _children;
};

} // namespace mv::util