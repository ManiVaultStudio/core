// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

namespace mv {

namespace gui {

class GroupAction;
class GroupWidgetTreeItem;

/**
 * Group section tree item class
 *
 * Tree item class for displaying a section push button with which the group contents can be expanded/collapsed
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT GroupSectionTreeItem : public QTreeWidgetItem
{
public:

    /**
     * Section push button class
     *
     * Push button for expanding/collapsing items in a groups action
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT PushButton : public QPushButton {
    public:

        /**
         * Constructor
         * @param treeWidgetItem Pointer to tree widget item
         * @param groupAction Pointer to group action
         * @param parent Pointer to parent widget
         */
        PushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, QWidget* parent = nullptr);

        /**
         * Respond to target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override final;

    protected:
        GroupAction*            _widgetActionGroup;         /** Pointer to widget action group */
        QTreeWidgetItem*        _parentTreeWidgetItem;      /** Pointer to parent tree widget item */
        QTreeWidgetItem*        _groupTreeWidgetItem;       /** Pointer to group tree widget item */
        QWidget*                _groupWidget;               /** Pointer to group widget */
        QWidget                 _overlayWidget;             /** Overlay widget for buttons etc. */
        QHBoxLayout             _overlayLayout;             /** Overlay layout */
        QLabel                  _iconLabel;                 /** Left-aligned icon label */
    };

public:

    /**
     * Constructor
     * @param treeWidget Pointer to tree widget
     * @param groupAction Pointer to group action (used for toggling)
     */
    GroupSectionTreeItem(QTreeWidget* treeWidget, GroupAction* groupAction);

    /** Destructor */
    ~GroupSectionTreeItem();

    /**
     * Get section push button
     * @return Reference to section push button
     */
    PushButton& getPushButton() const;

    /**
     * Get group action
     * @return Pointer to group action
     */
    GroupAction* getGroupAction() const;

protected:
    GroupAction*            _groupAction;               /** Pointer to group action (used for toggling) */
    PushButton*             _pushButton;                /** Push button for toggling expansion */
    GroupWidgetTreeItem*    _groupWidgetTreeItem;       /** Pointer to child group widget tree item */
};

}
}
