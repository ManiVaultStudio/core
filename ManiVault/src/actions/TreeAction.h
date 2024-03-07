// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QWidget>

class QAbstractItemModel;
class QSortFilterProxyModel;

namespace mv::gui {

/**
 * Tree action class
 *
 * Action class for interacting with a tree model
 *
 * @author Thomas Kroes
 */
class TreeAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        FilterToolbar = 0x00001,          /** Enable push button icon */
    };

public:

    /** Widget class for tree action */
    class Widget : public WidgetActionWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, \p treeAction and \p widgetFlags
         * @param parent Pointer to parent widget
         * @param treeAction Pointer to tree action
         * @param widgetFlags Widget flags
         */
        Widget(QWidget* parent, TreeAction* treeAction, const std::int32_t& widgetFlags);

    protected:
        TreeAction*     _treeAction;    /** Pointer to tree action */

        friend class TreeAction;
    };

protected:

    /**
     * Get widget representation of the tree action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TreeAction(QObject* parent, const QString& title);

    /**
     * Initializes the tree action with a pointer to a \p model, possibly a \p filterModel and \p itemTypeName
     * @param model Pointer to model
     * @param filterModel Pointer to filter model (maybe nullptr)
     * @param itemTypeName Item type name string
     */
    void initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName);

    /**
     * Get model
     * @return Pointer to model (maybe nullptr)
     */
    QAbstractItemModel* getModel() {
        return _model;
    }

    /**
     * Get filter model
     * @return Pointer to filter model (maybe nullptr)
     */
    QSortFilterProxyModel* getFilterModel() {
        return _filterModel;
    }

    /**
     * Get item type name
     * @return Item type name string
     */
    QString getItemTypeName() {
        return _itemTypeName;
    }

private:
    QAbstractItemModel*     _model;             /** Pointer to model */
    QSortFilterProxyModel*  _filterModel;       /** Pointer to filter model */
    QString                 _itemTypeName;      /** String that describes an individual model item type */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::TreeAction)

inline const auto treeActionMetaTypeId = qRegisterMetaType<mv::gui::TreeAction*>("mv::gui::TreeAction");
