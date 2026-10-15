// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ToolbarAction.h"

#include <QTimer>

namespace mv::gui {

/**
 * Horizontal toolbar action class
 *
 * Widget action for displaying actions in a horizontal toolbar
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HorizontalToolbarAction : public ToolbarAction
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
         * @param horizontalToolbarAction Pointer to horizontal toolbar action that creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, HorizontalToolbarAction* horizontalToolbarAction, const std::int32_t& widgetFlags);

        /**
         * Respond to \p target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        /** Set action widget to display */
        void setActionWidgets();

        /** Computes the states of the action widgets */
        void updateLayout();

    protected:
        HorizontalToolbarAction*    _horizontalToolbarAction;   /** Pointer to horizontal toolbar action that creates the widget */
        QHBoxLayout*                _layout;                    /** Main layout */
        QHBoxLayout*                _toolbarLayout;             /** Toolbar layout */
        QWidget                     _toolbarWidget;             /** Toolbar widget */
        QTimer                      _timer;                     /** Timer to periodically update the layout */

        friend class HorizontalToolbarAction;
    };

protected: // Widgets

    /**
     * Get widget representation of the horizontal toolbar action
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
     * @param alignment Item alignment (Qt::AlignmentFlag::AlignLeft, Qt::AlignmentFlag::AlignCenter or Qt::AlignmentFlag::AlignRight)
     */
    Q_INVOKABLE HorizontalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment = Qt::AlignmentFlag::AlignLeft);
};

}

Q_DECLARE_METATYPE(mv::gui::HorizontalToolbarAction)

inline const auto horizontalToolbarActionMetaTypeId = qRegisterMetaType<mv::gui::HorizontalToolbarAction*>("mv::gui::HorizontalToolbarAction");