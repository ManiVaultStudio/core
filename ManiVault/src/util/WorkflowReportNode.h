// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMessage.h"

#include <QString>
#include <QDateTime>

namespace mv::util
{

class WorkflowReportNode
{
public:
    using Ptr = std::shared_ptr<WorkflowReportNode>;

    explicit WorkflowReportNode(const QString& name, WorkflowReportNode* parent = nullptr)
        : _name(name)
        , _parent(parent)
    {
    }

    Ptr createChild(const QString& name)
    {
        QMutexLocker lock(&_mutex);

        auto child = std::make_shared<WorkflowReportNode>(name, this);

    	_children.push_back(child);

        return child;
    }

    void addInfo(const QString& source, const QString& text)
    {
        addMessage(WorkflowMessageLevel::Info, source, text);
    }

    void addWarning(const QString& source, const QString& text)
    {
        addMessage(WorkflowMessageLevel::Warning, source, text);
    }

    void addError(const QString& source, const QString& text)
    {
        addMessage(WorkflowMessageLevel::Error, source, text);
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

    WorkflowReportNode* getParent() const
    {
        return _parent;
    }

    bool hasErrorsRecursive() const
    {
        QMutexLocker lock(&_mutex);

        for (const auto& message : _messages) {
            if (message._level == WorkflowMessageLevel::Error)
                return true;
        }

        const auto children = _children;
        lock.unlock();

        for (const auto& child : children) {
            if (child->hasErrorsRecursive())
                return true;
        }

        return false;
    }

    int getWarningCountRecursive() const
    {
        int result = 0;

        QMutexLocker lock(&_mutex);

        for (const auto& message : _messages) {
            if (message._level == WorkflowMessageLevel::Warning)
                ++result;
        }

        const auto children = _children;
        lock.unlock();

        for (const auto& child : children)
            result += child->getWarningCountRecursive();

        return result;
    }

    int getErrorCountRecursive() const
    {
        int result = 0;

        QMutexLocker lock(&_mutex);

        for (const auto& message : _messages) {
            if (message._level == WorkflowMessageLevel::Error)
                ++result;
        }

        const auto children = _children;
        lock.unlock();

        for (const auto& child : children)
            result += child->getErrorCountRecursive();

        return result;
    }

private:
    void addMessage(WorkflowMessageLevel level, const QString& source, const QString& text)
    {
        QMutexLocker lock(&_mutex);
        _messages.push_back(WorkflowMessage{
            level,
            source,
            text,
            QDateTime::currentDateTime()
            });
    }

private:
    QString              _name;
    WorkflowReportNode* _parent = nullptr;

    mutable QMutex       _mutex;
    QVector<WorkflowMessage> _messages;
    QVector<Ptr>             _children;
};

} // namespace mv::util