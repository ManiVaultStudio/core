// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "WidgetAction.h"

#include <QTreeWidget>
#include <QVBoxLayout>

class QWidget;

namespace mv::gui {

class GroupAction;
class GroupSectionTreeItem;

/**
 * Group widget tree item class
 * 
 * Tree item class for displaying the contents of a group action
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT GroupWidgetTreeItem : public QTreeWidgetItem
{
    /**
    * Size synchronizer class
    *
    * A buq in QT exists where a tree view row height is not updated when the widget contents of that row changes.
    * This class detects changes in the row widget and sets the tree item size hint accordingly.
    *
    * @author Thomas Kroes
    */
    class CORE_EXPORT SizeSynchronizer : public QObject
    {
    public:

        /**
         * Constructor
         * @param groupTreeItem Pointer to group tree item
         */
        SizeSynchronizer(GroupWidgetTreeItem* groupTreeItem);

        /**
         * Respond to target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override final;

    protected:
        GroupWidgetTreeItem*  _groupTreeItem;   /** Pointer to group tree item */
    };

public:

    /**
     * Constructor
     * @param groupSectionTreeItem Pointer to group section tree widget item
     * @param groupAction Pointer to group action
     */
    GroupWidgetTreeItem(GroupSectionTreeItem* groupSectionTreeItem, GroupAction* groupAction);

    /** Destructor */
    ~GroupWidgetTreeItem();

    /**
     * Get group section tree item
     * @return Pointer to group section tree item
     */
    GroupSectionTreeItem* getGroupSectionTreeItem();

    /**
     * Get section width
     * @return Section width
     */
    std::int32_t getGroupSectionWidth() const;

    /**
     * Get group widget
     * @return Pointer to group widget
     */
    QWidget* getGroupWidget();

protected:
    GroupSectionTreeItem*   _groupSectionTreeItem;      /** Pointer to group section tree item */
    GroupAction*            _groupAction;               /** Pointer to group action */
    QWidget*                _containerWidget;           /** Pointer to container widget */
    QVBoxLayout*            _containerLayout;           /** Pointer to container widget layout */
    QPointer<QWidget>       _groupWidget;               /** Pointer to group widget */
    SizeSynchronizer        _sizeSynchronizer;          /** Synchronizer for widget size */
};

}
