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

    // Update select actions read only status
    const auto updateSelectActionsReadOnly = [this]() -> void {
        const auto numberOfRows         = _filterModel.rowCount();
        const auto selectedRows         = _selectionModel.selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();
        const auto hasSelection         = !selectedRows.isEmpty();

        _selectAllAction.setEnabled(numberOfSelectedRows != numberOfRows);
        _selectNoneAction.setEnabled(numberOfSelectedRows >= 1);
        _selectInvertAction.setEnabled(numberOfRows >= 1 );
    };

    // Update select actions read only status when the model selection or layout changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, updateSelectActionsReadOnly);
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, updateSelectActionsReadOnly);

    // Do an initial update of the actions read only status
    updateSelectActionsReadOnly();

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_filterModel.rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
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
