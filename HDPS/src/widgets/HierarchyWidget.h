#pragma once

#include "actions/StringAction.h"
#include "actions/TriggerAction.h"
#include "actions/GroupAction.h"
#include "actions/ToggleAction.h"

#include "OverlayWidget.h"

#include <QWidget>
#include <QTreeView>
#include <QAbstractItemModel>

class QSortFilterProxyModel;

namespace hdps
{

namespace gui
{

/**
 * Base widget for displaying a hierarchical model in a tree view
 * 
 * It sets up the tree view and links up the filter and selection models.
 * 
 * Features a toolbar for interaction with the model built with actions:
 * - Filter by name (possibly with regular expression)
 * - Expanding all items
 * - Collapsing all items
 * - Selecting all items
 * - De-selecting all items
 * - Edit settings
 * 
 * In addition, it shows an overlay widget (with icon, title and description) when no items are loaded
 * or filtering did not yield any items.
 * 
 * @author Thomas Kroes
 */
class HierarchyWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param itemTypeName Type name of the item (e.g. dataset or action)
     * @param model Reference to input model
     * @param filterModel Pointer to input filter model (if present)
     * @param showToolbar Whether to add a default toolbar for filtering and expand/collapse
     * @param showOverlay Whether to show an overlay when the (filter) model is empty
     */
    HierarchyWidget(QWidget* parent, const QString& itemTypeName, QAbstractItemModel& model, QSortFilterProxyModel* filterModel = nullptr, bool showToolbar = true, bool showOverlay = true);

    /**
     * Override set window icon to update the overlay icons as well
     * @param icon Window icon
     */
    void setWindowIcon(const QIcon& icon);

    /**
     * Set overlay widget description when no items are loaded
     * @param noItemsDescription Overlay widget description when no items are loaded
     */
    void setNoItemsDescription(const QString& noItemsDescription);

    /**
     * Get input model
     * @return Input model
     */
    QAbstractItemModel& getModel() {
        return _model;
    }

    /**
     * Get input filter model
     * @return Pointer to input filter model
     */
    QSortFilterProxyModel* getFilterModel() {
        return _filterModel;
    }

    /**
     * Get selection model
     * @return Reference to selection model
     */
    QItemSelectionModel& getSelectionModel() {
        return _selectionModel;
    }

    /**
     * Get tree view widget
     * @return Reference to tree view widget
     */
    QTreeView& getTreeView() {
        return _treeView;
    }

    /**
     * Get filter name action
     * @return Reference to filter name action
     */
    StringAction& getFilterNameAction() {
        return _filterNameAction;
    }

    /**
     * Get filter group action
     * @return Reference to filter group action
     */
    GroupAction& getFilterGroupAction() {
        return _filterGroupAction;
    }

    /**
     * Get filter regular expression action
     * @return Reference to filter regular expression action
     */
    ToggleAction& getFilterRegularExpressionAction() {
        return _filterRegularExpressionAction;
    }

    /**
     * Get filter case sensitive action
     * @return Reference to filter case sensitive action
     */
    ToggleAction& getFilterCaseSensitiveAction() {
        return _filterCaseSensitiveAction;
    }

    /**
     * Get expand all action
     * @return Reference to expand all action
     */
    TriggerAction& getExpandAllAction() {
        return _expandAllAction;
    }

    /**
     * Get collapse all action
     * @return Reference to collapse all action
     */
    TriggerAction& getCollapseAllAction() {
        return _collapseAllAction;
    }

    /**
     * Get selection group action
     * @return Reference to selection group
     */
    GroupAction& getSelectionGroupAction() {
        return _selectionGroupAction;
    }

    /**
     * Get selection group action
     * @return Reference to selection group
     */
    GroupAction& getSettingsGroupAction() {
        return _settingsGroupAction;
    }

    /**
     * Maps a model index to source model index
     * @param modelIndex Model index to map
     * @return The mapped model index if a filter model is present, other wise the input model index
     */
    QModelIndex toSourceModelIndex(const QModelIndex& modelIndex) const;

public:

    /**
     * Get selected rows
     * @return Model indices of the selected rows
     */
    QModelIndexList getSelectedRows() const;

    /**
     * Fetches filter model indices of \p filterModelIndex and its children recursively
     * @param filterModelIndex Filter model index
     * @return Fetched filter model indices
     */
    QModelIndexList fetchFilterModelIndices(QModelIndex filterModelIndex = QModelIndex()) const;

protected: // Item expansion

    /**
     * Get whether one or more items may be expanded 
     * @return Boolean indicating whether one or more items may be expanded
     */
    bool mayExpandAll() const;

    /** Expand all items in the hierarchy */
    void expandAll();

    /**
     * Get whether one or more items may be collapse
     * @return Boolean indicating whether one or more items may be collapse
     */
    bool mayCollapseAll() const;

    /** Collapse all items in the hierarchy */
    void collapseAll();

protected: // Item selection

    /**
     * Get whether all items may be selected
     * @return Boolean determining whether all items may be selected
     */
    bool maySelectAll() const;

    /** Select all items in the hierarchy */
    void selectAll();

    /**
     * Get whether one (or more) items can be de-selected
     * @return Boolean determining whether one (or more) items can be de-selected
     */
    bool maySelectNone() const;

    /** Deselect all selected items in the hierarchy */
    void selectNone();

private:
    
    /** Updates the overlay widget icon, title and description based on the state of the hierarchy */
    void updateOverlayWidget();

    /** Update the read-only state of the expand/collapse action in response to change in the model and the tree view */
    void updateExpandCollapseActionsReadOnly();

private:
    QString                     _itemTypeName;                      /** Name of the item type */
    QAbstractItemModel&         _model;                             /** Model containing data to be displayed in the hierarchy */
    QSortFilterProxyModel*      _filterModel;                       /** Pointer to filter model (maybe nullptr) */
    QItemSelectionModel         _selectionModel;                    /** Selection model */
    QTreeView                   _treeView;                          /** Tree view that contains the data hierarchy */
    OverlayWidget               _overlayWidget;                     /** Overlay widget that show information when there are no items in the model */
    QString                     _noItemsDescription;                /** Overlay widget description when no items are loaded */
    StringAction                _filterNameAction;                  /** String action for filtering by name */
    GroupAction                 _filterGroupAction;                 /** Filter group action */
    ToggleAction                _filterCaseSensitiveAction;         /** Filter case-sensitive action */
    ToggleAction                _filterRegularExpressionAction;     /** Enable filter with regular expression action */
    TriggerAction               _expandAllAction;                   /** Expand all items action */
    TriggerAction               _collapseAllAction;                 /** Collapse all items action */
    TriggerAction               _selectAllAction;                   /** Select all action */
    TriggerAction               _selectNoneAction;                  /** Select none action */
    GroupAction                 _selectionGroupAction;              /** Selection group action */
    GroupAction                 _settingsGroupAction;               /** Settings group action */
};

}
}
