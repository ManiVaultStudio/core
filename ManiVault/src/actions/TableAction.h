// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "VerticalGroupAction.h"
#include "StringAction.h"
#include "ModelFilterAction.h"

#include <QWidget>
#include <QTableView>

class QAbstractItemModel;
class QSortFilterProxyModel;

namespace mv::gui {

/**
 * Table action class
 *
 * Action class for interacting with a list model in a table view
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TableAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /** Widget class for table action */
    class CORE_EXPORT Widget : public WidgetActionWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, \p treeAction and \p widgetFlags
         * @param parent Pointer to parent widget
         * @param tableAction Pointer to owning table action
         * @param widgetFlags Widget flags
         */
        Widget(QWidget* parent, TableAction* tableAction, const std::int32_t& widgetFlags);

    private:

        /** Update the filter model and related actions */
        void updateFilterModel();

        /** Updates the overlay widget icon, title and description based on the state of the hierarchy */
        void updateOverlayWidget();

    protected:
        TableAction*            _tableAction;           /** Pointer to owning table action */
        HorizontalGroupAction   _toolbarGroupAction;    /** Horizontal toolbar group action */
        ModelFilterAction       _modelFilterAction;     /** Action for configuring filtering */
        QTableView              _tableView;             /** Table view for showing the data */

        friend class TableAction;
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
    Q_INVOKABLE TableAction(QObject* parent, const QString& title);

    /**
     * Initializes the table action with a pointer to a \p model, possibly a \p filterModel and \p itemTypeName
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

Q_DECLARE_METATYPE(mv::gui::TableAction)

inline const auto tableActionMetaTypeId = qRegisterMetaType<mv::gui::TableAction*>("mv::gui::TableAction");