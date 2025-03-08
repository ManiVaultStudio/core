// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatusAction.h"

#include <QTimer>

using namespace mv::util;

namespace mv::gui {

StatusAction::StatusAction(QObject* parent, const QString& title, const Status& status /*= Status::Undefined*/, const QString& message /*= ""*/) :
    WidgetAction(parent, "Title")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(status, message);
}

void StatusAction::initialize(const Status& status /*= Status::Info*/, const QString& message /*= ""*/)
{
    setStatus(status);
    setMessage(message);
}

StatusAction::Status StatusAction::getStatus() const
{
    return _status;
}

void StatusAction::setStatus(const Status& status)
{
    if (status == _status)
        return;

    _status = status;

    emit statusChanged(_status);
}

QString StatusAction::getMessage() const
{
    return _message;
}

void StatusAction::setMessage(const QString& message, bool vanish /*= false*/)
{
    if (message == _message)
        return;

    _message = message;

    emit messageChanged(_message);

    if (!vanish)
        return;

    // Reset the message after the disappear delay
    QTimer::singleShot(MESSAGE_DISAPPEAR_INTERVAL, [this]() {
        setMessage("");
    });
}

StatusAction::LineEditWidget::LineEditWidget(QWidget* parent, StatusAction* statusAction, const std::int32_t& widgetFlags) :
    QLineEdit(parent),
    _statusAction(statusAction),
    _trailingAction(this)
{
    setEnabled(false);

    _trailingAction.setEnabled(false);

    // Add the trailing action
    addAction(&_trailingAction, QLineEdit::TrailingPosition);

    // Update status
    const auto updateStatus = [this]() -> void {
        switch (_statusAction->getStatus())
        {
            case Status::Undefined:
            case Status::Info:
                break;

            case Status::Warning:
            case Status::Error:
                _trailingAction.setIcon(StyledIcon("exclamation-circle"));
                break;
        }
    };

    // Update status when the action status changed
    connect(statusAction, &StatusAction::statusChanged, this, updateStatus);

    // Update text
    const auto updateMessage = [this]() -> void {
        setText(_statusAction->getMessage());
    };

    // Update text when the action message changes
    connect(statusAction, &StatusAction::messageChanged, this, updateMessage);

    // Perform an initial update of the status and the message
    updateStatus();
    updateMessage();
}

}
