// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QProgressBar>

namespace hdps::gui {

/**
 * Progress action class
 *
 * Action class for displaying progress
 *
 * @author Thomas Kroes
 */
class ProgressAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        Horizontal  = 0x00001,  /** Widget includes a horizontal progress bar */
        Vertical    = 0x00002,  /** Widget includes a vertical progress bar */

        Default = Horizontal
    };

public:

    /** Progress bar widget for progress bar action */
    class ProgressBarWidget : public QProgressBar
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param progressAction Pointer to progress action
         * @param widgetFlags Widget flags
         */
        ProgressBarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags);

    protected:
        ProgressAction* _progressAction;      /** Pointer to progress action */

        friend class ProgressAction;
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
    Q_INVOKABLE ProgressAction(QObject* parent, const QString& title = "");

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::ProgressAction)

inline const auto progressActionMetaTypeId = qRegisterMetaType<hdps::gui::ProgressAction*>("hdps::gui::ProgressAction");
