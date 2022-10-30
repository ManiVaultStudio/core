#pragma once

#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

#include <QWidget>
#include <QTreeView>
#include <QAbstractItemModel>

class QSortFilterProxyModel;

namespace hdps
{

namespace gui
{

/**
 * Base widget for display a hierarchical model in a tree view
 * It sets up the tree view and links up the filter and selection models
 * Contains expand/collapse all actions
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
     * @param model Reference to input model
     * @param filterModel Pointer to input filter model (if present)
     * @param showToolbar Whether to add a default toolbar for filtering and expand/collapse
     */
    HierarchyWidget(QWidget* parent, QAbstractItemModel& model, QSortFilterProxyModel* filterModel = nullptr, bool showToolbar = true);

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
     * Get name filter action
     * @return Reference to name filter action
     */
    StringAction& getNameFilterAction() {
        return _nameFilterAction;
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
     * Maps a model index to source model index
     * @param modelIndex Model index to map
     * @return The mapped model index if a filter model is present, other wise the input model index
     */
    QModelIndex toSourceModelIndex(const QModelIndex& modelIndex) const;

protected:

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

private:
    QAbstractItemModel&         _model;                         /** Model containing data to be displayed in the hierarchy */
    QSortFilterProxyModel*      _filterModel;                   /** Pointer to filter model (maybe nullptr) */
    QItemSelectionModel         _selectionModel;                /** Selection model */
    QTreeView                   _treeView;                      /** Tree view that contains the data hierarchy */
    StringAction                _nameFilterAction;              /** String action for filtering by name */
    TriggerAction               _expandAllAction;               /** Expand all datasets action */
    TriggerAction               _collapseAllAction;             /** Collapse all datasets action */
    //NoDataOverlayWidget*        _noDataOverlayWidget;           /** Overlay help widget which is shown when no data is loaded */
};

}
}
