#include "GroupsAction.h"
#include "GroupSectionTreeItem.h"
#include "Application.h"
#include "widgets/Divider.h"

#include <QDebug>

namespace hdps {

namespace gui {

GroupsAction::GroupsAction(QObject* parent) :
    WidgetAction(parent),
    _groupActions()
{
    setDefaultWidgetFlags(Default);
}

void GroupsAction::addGroupAction(GroupAction* groupAction)
{
    qDebug() << "Add group action to groups action";

    // Add group action
    _groupActions << groupAction;

    // Pass through group action expanded signal
    connect(groupAction, &GroupAction::expanded, this, [this, groupAction]() -> void {
        emit groupActionExpanded(groupAction);
    });

    // Pass through group action collapsed signal
    connect(groupAction, &GroupAction::collapsed, this, [this, groupAction]() -> void {
        emit groupActionCollapsed(groupAction);
    });

    // Notify others that a group action was added
    emit groupActionAdded(groupAction);
}

void GroupsAction::removeGroupAction(GroupAction* groupAction)
{
    qDebug() << "Remove group action from groups action";

    if (!_groupActions.contains(groupAction))
        return;

    // Remove the group action
    _groupActions.removeOne(groupAction);

    // Remove connections to the group action
    disconnect(groupAction, &GroupAction::expanded, this, nullptr);
    disconnect(groupAction, &GroupAction::collapsed, this, nullptr);

    // Notify others that a group action was removed
    emit groupActionRemoved(groupAction);
}

void GroupsAction::setGroupActions(const GroupActions& groupActions)
{
    // Remove existing group actions
    resetGroupActions();

    // Iterate over all group actions and add them one-by-one
    for (auto groupAction : groupActions)
        addGroupAction(groupAction);
}

void GroupsAction::resetGroupActions()
{
    // Remove all group actions
    for (auto groupAction : _groupActions)
        if (groupAction != _groupActions.first())
            removeGroupAction(groupAction);
}

const GroupsAction::GroupActions& GroupsAction::getGroupActions() const
{
    return _groupActions;
}

bool GroupsAction::canExpandAll() const
{
    return getNumberOfExpandedGroupActions() < getGroupActions().count();
}

void GroupsAction::expandAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(true);
}

bool GroupsAction::canCollapseAll() const
{
    // Can collapse when the number of collapse group actions is larger than zero
    return getNumberOfExpandedGroupActions() >= 1;
}

void GroupsAction::collapseAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(false);
}

QVector<bool> GroupsAction::getExpansions() const
{
    QVector<bool> expansions;

    // Loop over all actions and add expansion state
    for (auto groupAction : _groupActions)
        expansions << groupAction->isExpanded();

    return expansions;
}

std::int32_t GroupsAction::getNumberOfExpandedGroupActions() const
{
    // Get group expansion states
    const auto expansions = getExpansions();

    // Count the number of expanded group actions
    return std::count_if(expansions.begin(), expansions.end(), [](auto expansion) -> bool { return expansion; });
}

GroupsAction::Widget::Widget(QWidget* parent, GroupsAction* groupsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupsAction, widgetFlags),
    _groupsAction(groupsAction),
    _layout(),
    _filteredActionsAction(this, true),
    _toolbarWidget(),
    _toolbarLayout(),
    _filterAction(this, "Search"),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default"),
    _factoryDefaultAction(this, "Factory default"),
    _treeWidget()
{
    // Configure layout
    _layout.setMargin(0);
    _layout.setAlignment(Qt::AlignTop);
    //_layout.setMargin(5);
    //_layout.setSpacing(3);
    //_layout.setAlignment(Qt::AlignTop);

    // Apply layout to widget
    setLayout(&_layout);

    // Create UI
    createToolbar(widgetFlags);
    createTreeWidget(widgetFlags);

    // Add filtering action to the groups action
    _groupsAction->addGroupAction(&_filteredActionsAction);

    // Perform an initial update of the toolbar and action filtering
    updateToolbar();
    updateFiltering();
}

void GroupsAction::Widget::createToolbar(const std::int32_t& widgetFlags)
{
    // Filtered actions may not be serialized
    _filteredActionsAction.setSerializable(false);

    // Filter action is in search mode
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

    // Configure toolbar layout
    _toolbarLayout.setContentsMargins(0, 2, 0, 2);
    _toolbarLayout.setSpacing(4);

    // Add toolbar items
    if (widgetFlags & Filtering)
        _toolbarLayout.addWidget(_filterAction.createWidget(this), 1);

    if (widgetFlags & Expansion) {
        if (widgetFlags & Filtering)
            _toolbarLayout.addWidget(createVerticalDivider());

        _toolbarLayout.addWidget(_expandAllAction.createWidget(this, TriggerAction::Icon));
        _toolbarLayout.addWidget(_collapseAllAction.createWidget(this, TriggerAction::Icon));
    }

    if (widgetFlags & Expansion) {
        if (widgetFlags & Filtering || widgetFlags & Expansion)
            _toolbarLayout.addWidget(createVerticalDivider());

        _toolbarLayout.addWidget(_loadDefaultAction.createWidget(this, TriggerAction::Icon));
        _toolbarLayout.addWidget(_saveDefaultAction.createWidget(this, TriggerAction::Icon));
        _toolbarLayout.addWidget(createVerticalDivider());
        _toolbarLayout.addWidget(_factoryDefaultAction.createWidget(this, TriggerAction::Icon));
    }

    // Set toolbar widget layout
    _toolbarWidget.setLayout(&_toolbarLayout);

    // And add to the main layout
    _layout.addWidget(&_toolbarWidget);

    // Update toolbar when the current dataset changes
    //connect(&_dataset, &Dataset<DatasetImpl>::changed, this, &DataPropertiesWidget::updateToolbar);

    // Update toolbar when one of the group actions is expanded/collapsed
    connect(_groupsAction, &GroupsAction::groupActionExpanded, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionCollapsed, this, &Widget::updateToolbar);

    // Update properties when the filter changes
    connect(&_filterAction, &StringAction::stringChanged, this, &Widget::updateFiltering);

    // Expand/collapse all when triggered
    connect(&_expandAllAction, &TriggerAction::triggered, _groupsAction, &GroupsAction::expandAll);
    connect(&_collapseAllAction, &TriggerAction::triggered, _groupsAction, &GroupsAction::collapseAll);
}

void GroupsAction::Widget::createTreeWidget(const std::int32_t& widgetFlags)
{
    // Configure tree widget
    _treeWidget.setHeaderHidden(true);
    _treeWidget.setIndentation(0);
    _treeWidget.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _treeWidget.setSelectionMode(QAbstractItemView::NoSelection);
    _treeWidget.setUniformRowHeights(false);
    _treeWidget.setEditTriggers(QTreeWidget::NoEditTriggers);

    // Remove border from tree widget
    _treeWidget.setStyleSheet("QTreeWidget { border: none; }");

    QStyleOption styleOption;

    styleOption.init(&_treeWidget);

    auto palette = _treeWidget.palette();

    palette.setColor(QPalette::Base, styleOption.palette.color(QPalette::Normal, QPalette::Button));

    _treeWidget.setPalette(palette);

    // Add tree widget to the layout
    _layout.addWidget(&_treeWidget);

    connect(_groupsAction, &GroupsAction::groupActionAdded, this, &Widget::addGroupAction);
    connect(_groupsAction, &GroupsAction::groupActionRemoved, this, &Widget::removeGroupAction);
}

void GroupsAction::Widget::updateToolbar()
{
    // Set toolbar read-only status based on whether a dataset is loaded
    _toolbarWidget.setEnabled(!_groupsAction->getGroupActions().isEmpty());

    // Set read-only states of actions
    _expandAllAction.setEnabled(_groupsAction->canExpandAll());
    _collapseAllAction.setEnabled(_groupsAction->canCollapseAll());
}

void GroupsAction::Widget::updateFiltering()
{
    qDebug() << "Updating action filtering";

    // Get filter string
    const auto filterString = _filterAction.getString();

    // Get group actions
    auto groupActions = _groupsAction->getGroupActions();

    // Do not include actions from the special filtering group action
    if (!groupActions.isEmpty())
        groupActions.removeFirst();

    // Found child widget actions
    QVector<WidgetAction*> foundActions;

    for (auto groupAction : groupActions)
        foundActions << groupAction->findChildren(filterString, false);

    // Update filtered actions group action
    _filteredActionsAction.setActions(foundActions);
    _filteredActionsAction.setText(foundActions.count() == 0 ? "No properties found" : QString("Found %1 properties").arg(foundActions.count()));
}

void GroupsAction::Widget::addGroupAction(GroupAction* groupAction)
{
    qDebug() << "Add group action" << groupAction->text();

    // Create new tree widget item for the section expand/collapse button
    auto sectionTreeWidgetItem = new GroupSectionTreeItem(&_treeWidget, groupAction);

    // Set expansion state from group action
    sectionTreeWidgetItem->setExpanded(groupAction->isExpanded());
}

void GroupsAction::Widget::removeGroupAction(GroupAction* groupAction)
{
    qDebug() << "Remove group action" << groupAction->text();

    // Go over all top-level tree items and remove the one with the matching group action
    for (std::int32_t topLevelTreeItemIndex = 0; topLevelTreeItemIndex < _treeWidget.topLevelItemCount(); topLevelTreeItemIndex++) {

        // Get pointer to top-level tree item
        const auto topLevelTreeItem = _treeWidget.topLevelItem(topLevelTreeItemIndex);

        // Only remove if we have a valid top-level item
        if (!topLevelTreeItem)
            continue;

        // Convert to group section tree item
        auto groupSectionTreeItem = dynamic_cast<GroupSectionTreeItem*>(topLevelTreeItem);

        // Only proceed if we have a valid pointer to a group section tree item
        if (!groupSectionTreeItem)
            continue;

        // Do not remove if not the same group action
        if (groupAction != groupSectionTreeItem->getGroupAction())
            continue;

        // Remove the top-level item from the tree widget
        //const auto takenTopLevelTreeItem = _treeWidget.takeTopLevelItem(topLevelTreeItemIndex);

        //// And delete the top-level tree item
        //if (takenTopLevelTreeItem)
        //    delete takenTopLevelTreeItem;
    }
}

}
}
