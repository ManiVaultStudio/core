// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"

#include "util/WidgetFader.h"

#include <QLabel>

class QDrag;

namespace mv::gui {

class WidgetAction;

/**
 * Widget action label class
 *
 * Provides a label view on the action
 * 
 * Features:
 * - Label of which the text, visibility and tooltip is synchronized with the action
 * - Provide linking commands
 *   - Publish an action (create a public shared action)
 *   - Connect to a shared action
 *   - Disconnect from a shared action
 * 
 * Note: This widget class is developed for internal use (not meant to be used explicitly in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class WidgetActionLabel : public WidgetActionViewWidget
{
public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ColonAfterName  = 0x00001,      /** The name label includes a post-fix colon */
    };

public:

    /**
     * Constructor
     * @param action Pointer to widget action
     * @param parent Pointer to parent widget
     * @param windowFlags Window flags
     */
    explicit WidgetActionLabel(WidgetAction* action, QWidget* parent = nullptr, const std::uint32_t& flags = ColonAfterName);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Invoked when the widget is resized
     * @param resizeEvent Pointer to the resize event
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

    /** Gets whether label elide is enabled or not */
    bool geElide() const;

    /**
     * Set whether label elide is enabled or not
     * @param labelElide Whether label elide is enabled or not
     */
    void setElide(bool elide);

private:

    /** Update the state of the name label */
    void updateNameLabel();

    /** Elides the label text */
    void elide();

    /**
     * Get label text (possibly with a post-fix colon)
     * @return Label text
     */
    QString getLabelText() const;
    
private slots:
    
    /**
     * Update custom theme parts not caught by the system itself
     */
    void updateCustomStyle() ;

protected:
    std::uint32_t       _flags;                     /** Configuration flags */
    QLabel              _nameLabel;                 /** Action name label */
    bool                _elide;                     /** Whether to enable label elide (e.g. whether to truncate text and show an ellipsis when there is insufficient space for the text) */
    QDrag*              _drag;                      /** Pointer is set during drag and drop action connection */
    QPoint              _lastMousePressPosition;    /** Location during last mouse press event */
};

}
