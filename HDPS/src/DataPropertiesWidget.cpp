#include "DataPropertiesWidget.h"

#include <Application.h>
#include <Core.h>
#include <DataHierarchyItem.h>
#include <actions/GroupAction.h>
#include <widgets/Divider.h>

#include <QDebug>
#include <QVBoxLayout>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _dataset(),
    _layout(),
    _groupsAction(this),
    _filteredActionsAction(this, true),
    _toolbarWidget(),
    _toolbarLayout(),
    _filterAction(this, "Search"),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default"),
    _factoryDefaultAction(this, "Factory default")
{
    setAutoFillBackground(true);

    _filteredActionsAction.setSerializable(false);

    _filterAction.setSearchMode(true);
    _filterAction.setPlaceHolderString("Filter properties by name...");

    // Set action icon
    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));
    _loadDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));
    _factoryDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("industry"));

    // Set action tooltips
    _filterAction.setToolTip("Filter properties by name");
    _expandAllAction.setToolTip("Expand all property sections");
    _collapseAllAction.setToolTip("Collapse all property sections");
    _loadDefaultAction.setToolTip("Load default properties");
    _saveDefaultAction.setToolTip("Save default properties");
    _factoryDefaultAction.setToolTip("Restore factory default properties");

    setLayout(&_layout);

    _layout.setMargin(5);
    _layout.setSpacing(3);
    _layout.setAlignment(Qt::AlignTop);

    // Configure toolbar layout
    _toolbarLayout.setContentsMargins(0, 2, 0, 2);
    _toolbarLayout.setSpacing(4);

    // Add toolbar items
    _toolbarLayout.addWidget(_filterAction.createWidget(this), 1);
    _toolbarLayout.addWidget(createVerticalDivider());
    _toolbarLayout.addWidget(_expandAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    _toolbarLayout.addWidget(_collapseAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    _toolbarLayout.addWidget(createVerticalDivider());
    _toolbarLayout.addWidget(_loadDefaultAction.createWidget(this, ToggleAction::PushButtonIcon));
    _toolbarLayout.addWidget(_saveDefaultAction.createWidget(this, ToggleAction::PushButtonIcon));
    _toolbarLayout.addWidget(createVerticalDivider());
    _toolbarLayout.addWidget(_factoryDefaultAction.createWidget(this, ToggleAction::PushButtonIcon));

    _toolbarWidget.setLayout(&_toolbarLayout);

    // Update toolbar when the current dataset changes
    connect(&_dataset, &Dataset<DatasetImpl>::changed, this, &DataPropertiesWidget::updateToolbar);

    // Update toolbar when one of the group actions is expanded/collapsed
    connect(&_groupsAction, &GroupsAction::expanded, this, &DataPropertiesWidget::updateToolbar);
    connect(&_groupsAction, &GroupsAction::collapsed, this, &DataPropertiesWidget::updateToolbar);

    // Add toolbar layout and groups widget
    _layout.addWidget(&_toolbarWidget);
    _layout.addWidget(_groupsAction.createWidget(this));

    emit currentDatasetGuiNameChanged("");

    // Update properties when the filter changes
    connect(&_filterAction, &StringAction::stringChanged, this, &DataPropertiesWidget::updateProperties);

    // Expand/collapse all when triggered
    connect(&_expandAllAction, &TriggerAction::triggered, this, &DataPropertiesWidget::expandAll);
    connect(&_collapseAllAction, &TriggerAction::triggered, this, &DataPropertiesWidget::collapseAll);

    // Perform an initial update of the toolbar
    updateToolbar();
}

void DataPropertiesWidget::setDatasetId(const QString& datasetId)
{
    try
    {
        // Disconnect any previous connection to data hierarchy item
        if (_dataset.isValid())
            disconnect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

        // Assign the dataset reference
        if (!datasetId.isEmpty())
            _dataset = Application::core()->requestDataset(datasetId);
        else
            _dataset.reset();

        // Only proceed if we have a valid reference
        if (_dataset.isValid())
        {
            // Reload when actions are added on-the-fly
            connect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
                if (dynamic_cast<GroupAction*>(&widgetAction) == nullptr)
                    return;

                loadDataset();
            });
        }

        // Initial dataset load
        loadDataset();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Cannot update data properties", e);
    }
    catch (...) {
        exceptionMessageBox("Cannot update data properties");
    }
}

void DataPropertiesWidget::loadDataset()
{
    // Inform others that the loaded dataset changed
    emit currentDatasetGuiNameChanged(_dataset.isValid() ? _dataset->getDataHierarchyItem().getFullPathName() : "");

    // Clear groups if the reference is invalid
    if (!_dataset.isValid()) {
        _groupsAction.set(QVector<GroupAction*>());
        return;
    }

    // Update toolbar and properties
    updateToolbar();
    updateProperties();
}

void DataPropertiesWidget::updateToolbar()
{
    // Set toolbar read-only status based on whether a dataset is loaded
    _toolbarWidget.setEnabled(_dataset.isValid());

    // Cannot update actions when the dataset is invalid
    if (!_dataset.isValid())
        return;

    // Set read-only states of actions
    _expandAllAction.setEnabled(canExpandAll());
    _collapseAllAction.setEnabled(canCollapseAll());
}

void DataPropertiesWidget::updateProperties()
{
    // Get filter string
    const auto filterString = _filterAction.getString();

    // Get group actions
    QVector<GroupAction*> groupActions;
    
    if (filterString.isEmpty())
        groupActions << getGroupActions();
    else
        groupActions << &_filteredActionsAction;

    // Assign the group actions to the accordion
    _groupsAction.set(groupActions);

    // Find child widget actions
    QVector<WidgetAction*> foundActions;

    // Get group actions
    auto filterGroupActions = getGroupActions();

    for (auto groupAction : filterGroupActions)
        foundActions << groupAction->findChildren(filterString, false);

    // Update filtered actions group action
    _filteredActionsAction.setActions(foundActions);
    _filteredActionsAction.setText(foundActions.count() == 0 ? "No properties found" : QString("Found %1 properties").arg(foundActions.count()));
}

bool DataPropertiesWidget::canExpandAll() const
{
    return getNumberOfExpandedGroupActions() < getGroupActions().count();
}

void DataPropertiesWidget::expandAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(true);
}

bool DataPropertiesWidget::canCollapseAll() const
{
    // Can collapse when the number of collapse group actions is larger than zero
    return getNumberOfExpandedGroupActions() >= 1;
}

void DataPropertiesWidget::collapseAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(false);
}

QVector<bool> DataPropertiesWidget::getExpansions() const
{
    QVector<bool> expansions;

    // Loop over all actions and add expansion state when the action is a group action
    for (auto action : _dataset->getDataHierarchyItem().getActions()) {
        if (action == _dataset->getDataHierarchyItem().getActions().last())
            continue;

        auto groupAction = dynamic_cast<GroupAction*>(action);

        // Add expansion state when the action is a group action
        if (groupAction)
            expansions << groupAction->isExpanded();
    }

    return expansions;
}

std::int32_t DataPropertiesWidget::getNumberOfExpandedGroupActions() const
{
    // Get group expansion states
    const auto expansions = getExpansions();

    // Count the number of expanded group actions
    return std::count_if(expansions.begin(), expansions.end(), [](auto expansion) -> bool { return expansion; });
}

QVector<GroupAction*> DataPropertiesWidget::getGroupActions() const
{
    QVector<GroupAction*> groupActions;

    // Get actions from data hierarchy item
    auto actions = _dataset->getDataHierarchyItem().getActions();

    // Loop over all actions and add action if is a group action
    for (auto action : actions) {
        auto groupAction = dynamic_cast<GroupAction*>(action);

        // Add expansion state when the action is a group action and if it meets the visibility constraint
        if (groupAction)
            groupActions << groupAction;
    }

    return groupActions;
}

}
}
