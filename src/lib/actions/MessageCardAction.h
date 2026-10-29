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
 * The action renders a category, severity, explanatory text, and any supplied
 * action buttons as an inline message card. Its message state is independent of
 * the generated widgets: updates are reflected in live widgets, and widgets
 * created later display the latest message. The parent QObject owns the action;
 * QWidgetAction manages the lifetime of generated widgets.
 *
 * Action buttons are presented in the order supplied to setMessage(). Pressing
 * a button emits actionTriggered() with its zero-based index; the owning plugin
 * decides what that action does.
 *
 * @author T. Kroes
 */
class CORE_EXPORT MessageCardAction : public WidgetAction
{
    Q_OBJECT

public:
    /** Describes one optional message button and the operation it performs. */
    struct ButtonAction
    {
        QString text;                 /**< Button label shown to the user. */
        std::function<void()> trigger; /**< Operation invoked when the button is pressed. */
    };

    using ButtonActions = QList<ButtonAction>;

    /** Constructs an empty message action. */
    explicit MessageCardAction(QObject* parent);

    /**
     * Replaces the displayed message and optional buttons.
     * @param severity Severity used for the icon, label, and card tint.
     * @param category Short category or heading shown beside the severity.
     * @param description Explanatory text displayed below the heading.
     * @param buttonActions Optional buttons and their callbacks, shown in list order.
     * Callbacks should not outlive any objects they capture; replace or clear
     * them when those objects are destroyed.
     */
    void setMessage(util::SeverityLevel severity, const QString& category, const QString& description, const ButtonActions& buttonActions = {});

protected:
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

private:
    void updateWidget(QWidget* widget);

    util::SeverityLevel      _severity = util::SeverityLevel::Info;
    QString                  _category;
    QString                  _description;
    ButtonActions            _buttonActions;
    QList<QPointer<QWidget>> _widgets;
};

}
