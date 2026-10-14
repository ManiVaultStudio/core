// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "models/AbstractActionsModel.h"
#include "models/ActionsFilterModel.h"
#include "widgets/HierarchyWidget.h"

#include <QWidget>
#include <QPointer>

class QMenu;

namespace mv::gui
{

/**
 * Actions widget class
 *
 * Widget class for viewing the contents of an actions model (e.g. list or hierarchy)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ActionsWidget final : public QWidget
{
    Q_OBJECT

    using RequestContextMenuFN = std::function<void(QMenu* menu, WidgetActions selectedActions)>;
    using PersistentModelIndices = QList<QPersistentModelIndex>;

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param actionsModel Reference to actions input model
     * @param itemTypeName Name of the action in the hierarchy widget
     */
    ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel, const QString& itemTypeName = "Parameter");

    /**
     * Get filter model
     * @return Reference to actions filter model
     */
    ActionsFilterModel& getFilterModel();

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    HierarchyWidget& getHierarchyWidget();

    /**
     * Sets the callback function which is called when the widget asks for a context menu
     * @param requestContextMenu Callback function which is called when the widget asks for a context menu
     */
    void setRequestContextMenuCallback(RequestContextMenuFN requestContextMenu);

private:
    
    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Invoked when rows are inserted and/or removed or when the model layout changes */
    void modelChanged();

    /**
     * Highlight actions
     * @param selectedRows Selected rows to highlight
     * @param highlight Whether to highlight or not
     */
    void highlightActions(const QModelIndexList& selectedRows, bool highlight);

    /**
     * Highlight actions
     * @param highlightModelIndices Model indices to highlight
     * @param highlight Whether to highlight or not
     */
    void highlightActions(const PersistentModelIndices& highlightModelIndices, bool highlight);

private:
    AbstractActionsModel&       _actionsModel;              /** Input actions model */
    ActionsFilterModel          _filterModel;               /** Hierarchical actions filter model */
    HierarchyWidget             _hierarchyWidget;           /** Widget for displaying action hierarchy */
    RequestContextMenuFN        _requestContextMenu;        /** Callback which is called when a context menu is requested */
    QPointer<QMenu>             _contextMenu;               /** Pointer to context menu (if any) */
    WidgetActions               _highlightedActions;        /** Pointers to currently highlighted actions */
};

}
