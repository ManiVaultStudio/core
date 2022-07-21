#pragma once

#include "DataHierarchyModel.h"
#include "DataHierarchyFilterModel.h"
#include "Dataset.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"

#include <QWidget>
#include <QTreeView>
#include <QGraphicsOpacityEffect>

namespace hdps
{

namespace gui
{

/**
* Widget for displaying the data hierarchy
*/
class DataHierarchyWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * No data overlay widget class
     *
     * @author Thomas Kroes
     */
    class NoDataOverlayWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        NoDataOverlayWidget(QWidget* parent);

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    protected:
        QGraphicsOpacityEffect*     _opacityEffect;     /** Effect for modulating opacity */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    DataHierarchyWidget(QWidget* parent);

protected:

    /**
     * Add a data hierarchy to the tree widget
     * @param dataHierarchyItem Reference to the data hierarchy item
     */
    void addDataHierarchyItem(DataHierarchyItem& dataHierarchyItem);

    /**
     * Get model index of the dataset
     * @param dataset Smart pointer to dataset
     * @return Dataset model index
     */
    QModelIndex getModelIndexByDataset(const Dataset<DatasetImpl>& dataset);

    /** Invoked when the number of rows changed (shows/hides the tree view header) */
    void numberOfRowsChanged();

protected:

    /**
     * Invoked when the dataset grouping action is toggled
     * @param toggled Whether dataset grouping is enabled or not
     */
    void onGroupingActionToggled(const bool& toggled);

    /** Update the visibility of the tree view columns */
    void updateColumnsVisibility();

    /**
     * Get indices
     * @param parent Parent model index
     */
    QModelIndexList getModelIndexList(QModelIndex parent = QModelIndex()) const;

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

    /** Called when the user expands an item in the tree view */
    void expanded(const QModelIndex& index);

    /** Called when the user collapses an item in the tree view */
    void collapsed(const QModelIndex& index);

protected:

    /** Updates the toolbar (enables/disables items) */
    void updateToolBar();

private:
    DataHierarchyModel          _model;                     /** Model containing data to be displayed in the hierarchy */
    DataHierarchyFilterModel    _filterModel;               /** Data hierarchy filter model */
    QTreeView                   _treeView;                  /** Tree view that contains the data hierarchy */
    QItemSelectionModel         _selectionModel;            /** Selection model */
    NoDataOverlayWidget*        _noDataOverlayWidget;       /** Overlay help widget which is shown when no data is loaded */
    StringAction                _datasetNameFilterAction;   /** String action for filtering dataset GUI name */
    TriggerAction               _expandAllAction;           /** Expand all datasets action */
    TriggerAction               _collapseAllAction;         /** Collapse all datasets action */
    ToggleAction                _groupingAction;            /** Data grouping action */
};

}
}
