#include "SelectClustersAction.h"
#include "ClustersFilterModel.h"

#include <QItemSelection>
#include <QHBoxLayout>

SelectClustersAction::SelectClustersAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    WidgetAction(parent),
    _filterModel(filterModel),
    _selectionModel(selectionModel),
    _selectAllAction(this, "All"),
    _selectNoneAction(this, "None"),
    _selectInvertAction(this, "Invert")
{
    setText("Select");
    setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _selectAllAction.setToolTip("Select all clusters");
    _selectNoneAction.setToolTip("De-select all clusters");
    _selectInvertAction.setToolTip("Invert the cluster selection");

    // Get item selection for entire filter model
    const auto getItemSelection = [this]() -> QItemSelection {
        const auto numberOfItems    = _filterModel.rowCount();
        const auto firstItemIndex   = _filterModel.index(0, 0);
        const auto lastItem         = _filterModel.index(numberOfItems - 1, 0);

        return QItemSelection(firstItemIndex, lastItem);
    };

    // Select all clusters when the select all action is triggered
    connect(&_selectAllAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    });

    // De-select all clusters when the select none action is triggered
    connect(&_selectNoneAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
    });

    // Invert the cluster selection when the select invert action is triggered
    connect(&_selectInvertAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
    });

    // Update selection actions read only status
    const auto updateActionsReadOnly = [this]() -> void {
        const auto numberOfRows         = _filterModel.rowCount();
        const auto selectedRows         = _selectionModel.selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();
        const auto hasSelection         = !selectedRows.isEmpty();

        _selectAllAction.setEnabled(numberOfSelectedRows != numberOfRows);
        _selectNoneAction.setEnabled(numberOfSelectedRows >= 1);
        _selectInvertAction.setEnabled(numberOfRows >= 1 );
    };

    // Update selection actions read only status when the model selection or layout changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, updateActionsReadOnly);
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, updateActionsReadOnly);

    // Do an initial update of the actions read only status
    updateActionsReadOnly();
}

SelectClustersAction::Widget::Widget(QWidget* parent, SelectClustersAction* selectClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, selectClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(selectClustersAction->getSelectAllAction().createWidget(this));
    layout->addWidget(selectClustersAction->getSelectNoneAction().createWidget(this));
    layout->addWidget(selectClustersAction->getSelectInvertAction().createWidget(this));

    setPopupLayout(layout);
}
