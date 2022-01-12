#include "FilterClustersAction.h"
#include "ClustersFilterModel.h"

#include <QItemSelection>
#include <QHBoxLayout>

FilterClustersAction::FilterClustersAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    WidgetAction(parent),
    _filterModel(filterModel),
    _selectionModel(selectionModel),
    _nameFilterAction(this, "Name filter")
{
    setText("Select");

    _nameFilterAction.setToolTip("Filter clusters by name (case-insensitive)");
    _nameFilterAction.setPlaceHolderString("Filter by cluster name...");

    // Update the name filter in the filter model
    const auto updateNameFilter = [this]() -> void {
        _filterModel.setNameFilter(_nameFilterAction.getString());
    };

    // Update the filter model filter when the name filter action string changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, updateNameFilter);

    // Do an initial update of the model name filter
    updateNameFilter();
}

FilterClustersAction::Widget::Widget(QWidget* parent, FilterClustersAction* filterClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, filterClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    layout->addWidget(filterClustersAction->getNameFilterAction().createWidget(this));

    setLayout(layout);
}
