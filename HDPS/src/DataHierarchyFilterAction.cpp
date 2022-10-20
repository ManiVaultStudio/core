#include "DataHierarchyFilterAction.h"
#include "DataHierarchyFilterModel.h"

#include <Application.h>

#include <QHBoxLayout>

using namespace hdps;

DataHierarchyFilterAction::DataHierarchyFilterAction(QObject* parent, DataHierarchyFilterModel& dataHierarchyFilterModel) :
    WidgetAction(parent),
    _dataHierarchyFilterModel(dataHierarchyFilterModel),
    _showHiddenAction(this, "Show hidden")
{
    setText("Filtering");
    setToolTip("Dataset filtering settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));

    _showHiddenAction.setToolTip("Show hidden datasets");

    connect(&_showHiddenAction, &ToggleAction::toggled, this, &DataHierarchyFilterAction::updateFilterModel);

    updateFilterModel();
}

void DataHierarchyFilterAction::updateFilterModel()
{
    _dataHierarchyFilterModel.setFilterHidden(!_showHiddenAction.isChecked());
}

DataHierarchyFilterAction::Widget::Widget(QWidget* parent, DataHierarchyFilterAction* dataHierarchyFilterAction, const std::int32_t& widgetFlags /*= 0*/) :
    WidgetActionWidget(parent, dataHierarchyFilterAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(dataHierarchyFilterAction->getShowHiddenAction().createWidget(this));

    if (widgetFlags & PopupLayout)
        setPopupLayout(layout);
    else
        setLayout(layout);
}