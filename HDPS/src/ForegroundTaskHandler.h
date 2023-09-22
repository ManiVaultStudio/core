// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/TasksAction.h"
#include "actions/ToggleAction.h"

#include <QToolButton>
#include <QMenu>
#include <QGraphicsOpacityEffect>

class QMainWindow;

namespace hdps {

/**
 * Foreground task handler class
 *
 * Contains a specialized tasks action that displays running foreground tasks.
 *
 * @author Thomas Kroes
 */
class ForegroundTaskHandler final : public AbstractTaskHandler
{
public:

    class StatusBarButton : public QToolButton {
    public:
        StatusBarButton(QWidget* parent = nullptr);

        /**
         * Paint event
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent);

    public: // Action getters

        gui::ToggleAction& getSeeThroughAction() { return _seeThroughAction; }

    private:
        QMenu               _menu;                  /** Popup menu attached to the tool button */
        gui::ToggleAction   _seeThroughAction;
    };

protected:

    class PopupWidget : public QWidget {
    public:
        PopupWidget(StatusBarButton* statusBarButton, QWidget* parent = nullptr);

        /**
         * Respond to target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

        QSize sizeHint() const override;

    private:

        /** Overlays the tasks icon with a badge which reflects the number of foreground tasks */
        void updateStatusBarButtonIcon();
        
        QMainWindow* getMainWindow();

        void synchronizeWithAnchor();

    private:
        StatusBarButton*        _statusBarButton;
        gui::TasksAction        _tasksAction;           /** Tasks action which will be configured to show running foreground tasks */
        QPixmap                 _tasksIconPixmap;       /** Tasks icon pixmap underlay (count badge will be drawn on top) */
        QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating label opacity */

        static const QSize iconPixmapSize;
    };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ForegroundTaskHandler(QObject* parent);

    /** Initializes the handler */
    void init() override;

    StatusBarButton& getStatusBarButton() { return _statusBarButton; }

private:
    StatusBarButton     _statusBarButton;
    PopupWidget         _popupWidget;
};

}
