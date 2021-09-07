#include "SubsetAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"

#include <QItemSelection>
#include <QHBoxLayout>

using namespace hdps;
using namespace hdps::gui;

SubsetAction::SubsetAction(QObject* parent, ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    WidgetAction(parent),
    _clustersAction(clustersAction),
    _filterModel(filterModel),
    _selectionModel(selectionModel),
    _subsetNameAction(this, "Subset name"),
    _createSubsetAction(this, "Create subset")
{
    setText("Subset");

    _subsetNameAction.setToolTip("Name of the subset");
    _createSubsetAction.setToolTip("Create the subset");

    _subsetNameAction.setPlaceHolderString("Enter subset name here...");

    // Get item selection for entire filter model
    const auto getItemSelection = [this]() -> QItemSelection {
        const auto numberOfItems    = _filterModel.rowCount();
        const auto firstItemIndex   = _filterModel.index(0, 0);
        const auto lastItem         = _filterModel.index(numberOfItems - 1, 0);

        return QItemSelection(firstItemIndex, lastItem);
    };

    connect(&_createSubsetAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _clustersAction.createSubset(_subsetNameAction.getString());
    });

    const auto updateSelectionActions = [this]() -> void {
        const auto selectedRows         = _selectionModel.selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();

        setEnabled(numberOfSelectedRows >= 1);
    };

    const auto updateCreateSubsetAction = [this]() -> void {
        _createSubsetAction.setEnabled(!_subsetNameAction.getString().isEmpty());
    };

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this, updateSelectionActions](const QItemSelection& selected, const QItemSelection& deselected) {
        updateSelectionActions();
    });

    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, [this, updateSelectionActions](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        updateSelectionActions();
    });

    connect(&_subsetNameAction, &StringAction::stringChanged, this, [this, updateCreateSubsetAction](const QString& string) {
        updateCreateSubsetAction();
    });

    updateSelectionActions();
    updateCreateSubsetAction();
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, subsetAction, state)
{
    auto mainLayout = new QHBoxLayout();

    mainLayout->setMargin(0);

    mainLayout->addWidget(subsetAction->getSubsetNameAction().createWidget(this));
    mainLayout->addWidget(subsetAction->getCreateSubsetAction().createWidget(this));

    setLayout(mainLayout);
}
