// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageAction.h"

#include <models/StandardItemModel.h>

#include <QMap>
#include <QString>

/**
 * Abstract page actions model class
 *
 * Abstract base class for page actions.
 *
 * @author Thomas Kroes
 */
class AbstractPageActionsModel : public mv::StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Title,  /** Title */

        Count
    };

    /** Base standard model item class for page action */
    class Item final : public QStandardItem, public QObject {
    public:

        /**
         * Construct with shared pointer to \p pageAction
         * @param pageAction Shared pointer to page action
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(const PageActionSharedPtr& pageAction, bool editable = false);

        /**
         * Get page action
         * return Shared pointer to the page action
         */
        PageActionSharedPtr getPageAction() const;

    private:
        const PageActionSharedPtr   _pageAction;    /** Shared pointer to page action */
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractPageActionsModel(QObject* parent = nullptr);

    /**
     * Add \p pageAction
     * @param pageAction Shared pointer to page action
     */
    virtual void add(const PageActionSharedPtr& pageAction) = 0;

    /**
     * Add \p pageAction
     * @param pageAction Shared pointer to page action
     */
    virtual void remove(const PageActionSharedPtr& pageAction) = 0;

    /** Resets the rows and notifies others */
    void reset();

private:
    PageActionSharedPtrs  _pageActions;   /** List of page actions */
};

