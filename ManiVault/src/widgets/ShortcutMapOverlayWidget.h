// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsOpacityEffect>

namespace mv::util {
    class ShortcutMap;
}

namespace mv::gui
{

/**
 * Shortcut map overlay widget class
 *
 * Overlays the source widget with a shortcut map cheat sheet
 *  
 * @author Thomas Kroes
 */
class ShortcutMapOverlayWidget : public OverlayWidget
{
private:

    /** Custom close label with mouse hover opacity */
    class CloseLabel : public QLabel
    {
    public:

        /**
         * Construct with pointer to owning \p shortcutMapOverlayWidget
         * @param shortcutMapOverlayWidget 
         */
        CloseLabel(ShortcutMapOverlayWidget* shortcutMapOverlayWidget);

        /**
         * Invoked when the mouse enters the toolbar item widget
         * @param event Pointer to enter event
         */
        void enterEvent(QEnterEvent* event) override;

        /**
         * Invoked when the mouse leaves the toolbar item widget
         * @param event Pointer to event
         */
        void leaveEvent(QEvent* event) override;

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

    private:
        ShortcutMapOverlayWidget*   _shortcutMapOverlayWidget;  /** Pinter to owning shortcut map overlay widget */
        QGraphicsOpacityEffect      _opacityEffect;             /** Effect for modulating label opacity */
    };

public:

    /**
     * Construct with pointer to \p source widget and \p shortcutMap
     * @param source Pointer to source widget
     * @param shortcutMap Const reference to the shortcut map for which to create the overlay
     */
    ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap);

private:
    const util::ShortcutMap&    _shortcutMap;       /** Const reference to the shortcut map for which to create the overlay */
    QVBoxLayout                 _mainLayout;        /** Main layout */
    QHBoxLayout                 _toolbarLayout;     /** Layout for the toolbar */
    CloseLabel                  _closeIconLabel;    /** Label for close icon */
    QHBoxLayout                 _headerLayout;      /** Layout for the header */
    QLabel                      _headerIconLabel;   /** Label for header icon */
    QLabel                      _headerTextLabel;   /** Label for header text */
    QLabel                      _bodyLabel;         /** Shortcut cheatsheet HTML */
};

}
