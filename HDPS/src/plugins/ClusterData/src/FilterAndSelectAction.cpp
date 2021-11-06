#include "FilterAndSelectAction.h"
#include "ClustersFilterModel.h"

#include <QTreeView>
#include <QItemSelection>
#include <QHBoxLayout>

using namespace hdps;
using namespace hdps::gui;

FilterAndSelectAction::FilterAndSelectAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    WidgetAction(parent),
    _filterModel(filterModel),
    _selectionModel(selectionModel),
    _nameFilterAction(this, "Name filter"),
    _selectAllAction(this, "All"),
    _selectNoneAction(this, "None"),
    _selectInvertAction(this, "Invert")
{
    setText("Select");

    _nameFilterAction.setToolTip("Filter clusters by name (case-insensitive)");
    _selectAllAction.setToolTip("Select all clusters");
    _selectNoneAction.setToolTip("De-select all clusters");
    _selectInvertAction.setToolTip("Invert the cluster selection");

    _nameFilterAction.setPlaceHolderString("Filter by cluster name...");

    // Update the name filter in the filter model
    const auto updateNameFilter = [this]() -> void {
        _filterModel.setNameFilter(_nameFilterAction.getString());
    };

    connect(&_nameFilterAction, &StringAction::stringChanged, this, [this, updateNameFilter](const QString& string) {
        updateNameFilter();
    });

    // Get item selection for entire filter model
    const auto getItemSelection = [this]() -> QItemSelection {
        const auto numberOfItems    = _filterModel.rowCount();
        const auto firstItemIndex   = _filterModel.index(0, 0);
        const auto lastItem         = _filterModel.index(numberOfItems - 1, 0);

        return QItemSelection(firstItemIndex, lastItem);
    };

    connect(&_selectAllAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    });

    connect(&_selectNoneAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
    });

    connect(&_selectInvertAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _selectionModel.select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
    });

    // Update selection action in response to model (selection) changes
    const auto updateSelectionActions = [this]() -> void {
        const auto numberOfRows         = _filterModel.rowCount();
        const auto selectedRows         = _selectionModel.selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();
        const auto hasSelection         = !selectedRows.isEmpty();

        _selectAllAction.setEnabled(numberOfSelectedRows != numberOfRows);
        _selectNoneAction.setEnabled(numberOfSelectedRows >= 1);
        _selectInvertAction.setEnabled(numberOfRows >= 1 );
    };

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this, updateSelectionActions](const QItemSelection& selected, const QItemSelection& deselected) {
        updateSelectionActions();
    });

    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, [this, updateSelectionActions](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        updateSelectionActions();
    });

    updateSelectionActions();
    updateNameFilter();
}

FilterAndSelectAction::Widget::Widget(QWidget* parent, FilterAndSelectAction* filterAndSelectAction) :
    WidgetActionWidget(parent, filterAndSelectAction)
{
    auto mainLayout = new QHBoxLayout();

    mainLayout->setMargin(0);

    auto nameFilterWidget   = filterAndSelectAction->getNameFilterAction().createWidget(this);
    auto selectAllWidget    = filterAndSelectAction->getSelectAllAction().createWidget(this);
    auto selectNoneWidget   = filterAndSelectAction->getSelectNoneAction().createWidget(this);
    auto selectInvertWidget = filterAndSelectAction->getSelectInvertAction().createWidget(this);

    const auto fixedPushButtonWidth = 50;

    selectAllWidget->setFixedWidth(fixedPushButtonWidth);
    selectNoneWidget->setFixedWidth(fixedPushButtonWidth);
    selectInvertWidget->setFixedWidth(fixedPushButtonWidth);

    mainLayout->addWidget(nameFilterWidget);
    mainLayout->addWidget(selectAllWidget);
    mainLayout->addWidget(selectNoneWidget);
    mainLayout->addWidget(selectInvertWidget);

    setLayout(mainLayout);
}
