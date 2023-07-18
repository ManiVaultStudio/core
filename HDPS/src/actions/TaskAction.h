// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Task action class
 *
 * Task action class for interacting with a task
 *
 * @author Thomas Kroes
 */
class TaskAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        ProgressBar = 0x00001,  /** Widget includes a progress bar */
        KillButton  = 0x00002,  /** Widget includes a button to kill the task */

        Default = ProgressBar | KillButton
    };

protected:

    /**
     * Get widget representation of the toggle action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TaskAction(QObject* parent, const QString& title = "");

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::TaskAction)

inline const auto taskActionMetaTypeId = qRegisterMetaType<hdps::gui::TaskAction*>("hdps::gui::TaskAction");
