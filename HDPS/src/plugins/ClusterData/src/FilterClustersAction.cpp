#include "FilterClustersAction.h"
#include "ClustersFilterModel.h"

#include <Application.h>

#include <QItemSelection>
#include <QHBoxLayout>

FilterClustersAction::FilterClustersAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    WidgetAction(parent),
    _filterModel(filterModel),
    _selectionModel(selectionModel),
    _nameFilterAction(this, "Name filter"),
    _clearNameFilterAction(this, "Clear name filter")
{
    setText("Filter clusters");
    setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));

    _nameFilterAction.setToolTip("Filter clusters by name (case-insensitive)");
    _nameFilterAction.setPlaceHolderString("Filter by cluster name...");

    _clearNameFilterAction.setToolTip("Clear the name filter");
    _clearNameFilterAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));

    // Update the name filter in the filter model
    const auto updateNameFilter = [this]() -> void {
        _filterModel.setNameFilter(_nameFilterAction.getString());
        _clearNameFilterAction.setEnabled(!_nameFilterAction.getString().isEmpty());
    };

    // Update the filter model filter when the name filter action string changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, updateNameFilter);

    // Clear the name filter when the clear name filter string changes
    connect(&_clearNameFilterAction, &TriggerAction::triggered, this, [this]() {
        _nameFilterAction.reset();
    });

    // Do an initial update of the model name filter
    updateNameFilter();
}

FilterClustersAction::Widget::Widget(QWidget* parent, FilterClustersAction* filterClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, filterClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(filterClustersAction->getNameFilterAction().createWidget(this));
    layout->addWidget(filterClustersAction->getClearNameFilterAction().createWidget(this, TriggerAction::Icon));

    setPopupLayout(layout);
}
