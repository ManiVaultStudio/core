// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WidgetAction.h"

#include "util/SeverityLevel.h"

#include <QList>
#include <QPointer>

#include <functional>

namespace mv::gui
{

/**
 * @brief Presents a severity-coded message card with optional user actions.
 *
 * The action renders a category, severity, explanatory text, and optional
 * buttons as an inline message card. It stores the message and callbacks
 * independently of its generated widgets, updating live widgets and applying
 * the latest state to widgets created later. A parent QObject owns the action
 * when supplied; otherwise, the creator manages its lifetime. QWidgetAction
 * manages the generated widgets.
 *
 * Each button callback is copied and retained until the message is replaced or
 * the action is destroyed. Callbacks run directly in response to button clicks
 * and are not exception guarded. Clear or replace callbacks before destroying
 * any objects they capture; callbacks must not throw.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT MessageCardAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes one message button and the operation it performs when clicked. */
    struct ButtonAction
    {
        QString                 text;    /**< Button label shown to the user. */
        std::function<void()>   trigger; /**< Operation invoked when the button is pressed. */
    };

    /** Ordered button definitions displayed by setMessage(). */
    using ButtonActions = QList<ButtonAction>;

    /** Constructs an empty Info message action. The parent, when supplied, owns this action. */
    explicit MessageCardAction(QObject* parent);

    /**
     * Replaces the stored message and button definitions, updating any live widgets.
     * @param severity Severity used for the icon, label, and card tint.
     * @param category Short category or heading shown beside the severity.
     * @param description Explanatory text displayed below the heading.
     * @param buttonActions Buttons and callbacks in display order. Each entry
     * creates a button; an empty callback leaves it without an operation.
     *
     * Callbacks are copied and retained until replaced or this action is
     * destroyed. They must not throw or outlive any objects they capture.
     */
    void setMessage(util::SeverityLevel severity, const QString& category, const QString& description, const ButtonActions& buttonActions = {});

protected:
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

private:
    void updateWidget(QWidget* widget);

private:
    util::SeverityLevel      _severity = util::SeverityLevel::Info; /**< Severity shown by the card; defaults to Info. */
    QString                  _category;                             /**< Short heading displayed beside the severity. */
    QString                  _description;                          /**< Explanatory text displayed below the heading. */
    ButtonActions            _buttonActions;                        /**< Current button labels and callbacks, in display order. */
    QList<QPointer<QWidget>> _widgets;                              /**< Generated card widgets updated when the message changes. */
};

}
