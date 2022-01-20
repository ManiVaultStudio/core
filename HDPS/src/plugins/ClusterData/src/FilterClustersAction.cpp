#include "FilterClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <Application.h>

#include <QHBoxLayout>

FilterClustersAction::FilterClustersAction(ClustersActionWidget* clustersActionWidget) :
    WidgetAction(clustersActionWidget),
    _clustersActionWidget(clustersActionWidget),
    _nameFilterAction(this, "Name filter"),
    _clearNameFilterAction(this, "Clear name filter")
{
    setText("Filter clusters");
    setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));

    _nameFilterAction.setToolTip("Filter clusters by name (case-insensitive)");
    _nameFilterAction.setPlaceHolderString("Filter by name...");

    _clearNameFilterAction.setToolTip("Clear the name filter");
    _clearNameFilterAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));

    // Update the name filter in the filter model
    const auto updateNameFilter = [this]() -> void {
        _clustersActionWidget->getFilterModel().setNameFilter(_nameFilterAction.getString());
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

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getFilterModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
}

FilterClustersAction::Widget::Widget(QWidget* parent, FilterClustersAction* filterClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, filterClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(filterClustersAction->getNameFilterAction().createWidget(this));
    layout->addWidget(filterClustersAction->getClearNameFilterAction().createWidget(this, TriggerAction::Icon));

    setPopupLayout(layout);
}
