// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ToolbarAction.h"

#include <QTimer>

namespace mv::gui {

/**
 * Vertical toolbar action class
 *
 * Widget action for displaying actions in a vertical toolbar
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT VerticalToolbarAction : public ToolbarAction
{
    Q_OBJECT

public: // Widgets

    /** Widget class for the horizontal toolbar */
    class CORE_EXPORT Widget final : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param verticalToolbarAction Pointer to vertical toolbar action that creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, VerticalToolbarAction* verticalToolbarAction, const std::int32_t& widgetFlags);

    private:

        /** Set action widget to display */
        void setActionWidgets();

    protected:
        VerticalToolbarAction* _verticalToolbarAction;      /** Pointer to vertical toolbar action that creates the widget */
        QVBoxLayout*            _layout;                    /** Main layout */
        QVBoxLayout*            _toolbarLayout;             /** Toolbar layout */
        QWidget                 _toolbarWidget;             /** Toolbar widget */

        friend class VerticalToolbarAction;
    };

protected: // Widgets

    /**
     * Get widget representation of the vertical toolbar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param alignment Item alignment
     */
    Q_INVOKABLE VerticalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment = Qt::AlignmentFlag::AlignTop);
};

}

Q_DECLARE_METATYPE(mv::gui::VerticalToolbarAction)

inline const auto verticalToolbarActionActionMetaTypeId = qRegisterMetaType<mv::gui::VerticalToolbarAction*>("mv::gui::VerticalToolbarAction");