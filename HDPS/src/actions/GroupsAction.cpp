#include "GroupsAction.h"
#include "GroupSectionTreeItem.h"
#include "Application.h"
#include "widgets/Divider.h"

#include <QDebug>

//#define _VERBOSE

namespace hdps {

namespace gui {

GroupsAction::GroupsAction(QObject* parent) :
    WidgetAction(parent),
    _groupActions(),
    _visibility()
{
    setDefaultWidgetFlags(Default);
}

void GroupsAction::addGroupAction(GroupAction* groupAction, bool visible /*= true*/)
{
#ifdef _VERBOSE
    qDebug().noquote() << QString("Add %1 to groups action").arg(groupAction->getSettingsPath());
#endif

    Q_ASSERT(groupAction != nullptr);

    // Add group action
    _groupActions << groupAction;

    // Set group action visibility
    _visibility[groupAction] = visible;

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
#ifdef _VERBOSE
    qDebug().noquote() << QString("Remove %1 from groups action").arg(groupAction->getSettingsPath());
#endif

    Q_ASSERT(_groupActions.contains(groupAction));
    Q_ASSERT(_visibility.contains(groupAction));

    if (!_groupActions.contains(groupAction))
        return;

    // Remove the group action
    _groupActions.removeOne(groupAction);

    // Remove from visibility as well
    if (_visibility.contains(groupAction))
        _visibility.remove(groupAction);

    // Remove connections to the group action
    disconnect(groupAction, &GroupAction::expanded, this, nullptr);
    disconnect(groupAction, &GroupAction::collapsed, this, nullptr);

    // Notify others that a group action was removed
    emit groupActionRemoved(groupAction);
}

void GroupsAction::setGroupActions(const GroupActions& groupActions)
{
#ifdef _VERBOSE
    qDebug() << "Set group actions";
#endif

    // Remove existing group actions
    resetGroupActions();

    // Iterate over all group actions and add them one-by-one
    for (auto groupAction : groupActions)
        addGroupAction(groupAction);
}

void GroupsAction::resetGroupActions()
{
#ifdef _VERBOSE
    qDebug() << "Reset group actions";
#endif

    // Cache group actions in order to avoid timing issues
    auto groupActions = _groupActions;

    // Remove all group actions except the filtered actions group action
    for (auto groupAction : groupActions)
        if (groupAction != groupActions.first())
            removeGroupAction(groupAction);
}

const GroupsAction::GroupActions& GroupsAction::getGroupActions() const
{
    return _groupActions;
}

bool GroupsAction::canExpandAll() const
{
    return getNumberOfExpandedGroupActions() < getNumberOfVisibleGroupActions();
}

void GroupsAction::expandAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(true);
}

bool GroupsAction::canCollapseAll() const
{
    return getNumberOfExpandedGroupActions() >= 1;
}

void GroupsAction::collapseAll()
{
    for (auto groupAction : getGroupActions())
        groupAction->setExpanded(false);
}

QVector<bool> GroupsAction::getExpansions(bool visibleOnly /*= true*/) const
{
    QVector<bool> expansions;

    // Loop over all actions and add expansion state
    for (auto groupAction : _groupActions) {
        if (visibleOnly) {
            if (_visibility[groupAction])
                expansions << groupAction->isExpanded();
        }
        else {
            expansions << groupAction->isExpanded();
        }
    }

    return expansions;
}

std::int32_t GroupsAction::getNumberOfExpandedGroupActions(bool visibleOnly /*= true*/) const
{
    // Get group expansion states
    const auto expansions = getExpansions(visibleOnly);

    // Count the number of expanded group actions
    return std::count_if(expansions.begin(), expansions.end(), [](auto expansion) -> bool { return expansion; });
}

std::int32_t GroupsAction::getNumberOfVisibleGroupActions() const
{
    return std::count_if(_visibility.begin(), _visibility.end(), [](auto visible) -> bool { return visible; });
}

void GroupsAction::setGroupActionVisibility(GroupAction* groupAction, bool visible)
{
    if (!_visibility.contains(groupAction))
        return;

    // Set group action visible
    _visibility[groupAction] = visible;

    // Notify others that the group action visibility changed
    if (visible)
        emit groupActionShown(groupAction);
    else
        emit groupActionHidden(groupAction);
}

void GroupsAction::showGroupAction(GroupAction* groupAction)
{
    setGroupActionVisibility(groupAction, true);
}

void GroupsAction::showAllGroupActions()
{
    for (auto groupAction : _groupActions)
        showGroupAction(groupAction);
}

void GroupsAction::hideGroupAction(GroupAction* groupAction)
{
    setGroupActionVisibility(groupAction, false);
}

void GroupsAction::hideAllGroupActions()
{
    for (auto groupAction : _groupActions)
        hideGroupAction(groupAction);
}

bool GroupsAction::isGroupActionVisible(GroupAction* groupAction) const
{
    if (!_visibility.contains(groupAction))
        return false;

    return _visibility[groupAction];
}

bool GroupsAction::isGroupActionHidden(GroupAction* groupAction) const
{
    return !isGroupActionVisible(groupAction);
}

void GroupsAction::loadDefault(bool recursive /*= true*/)
{
    for (auto groupAction : _groupActions)
        groupAction->loadDefault(recursive);
}

void GroupsAction::saveDefault(bool recursive /*= true*/)
{
    for (auto groupAction : _groupActions)
        groupAction->saveDefault(recursive);
}

bool GroupsAction::canSaveDefault(bool recursive /*= true*/) const
{
    for (auto groupAction : _groupActions)
        if (groupAction->canSaveDefault(recursive))
            return true;

    return false;
}

bool GroupsAction::isResettable(bool recursive /*= true*/) const
{
    for (auto groupAction : _groupActions)
        if (groupAction->isResettable(recursive))
            return true;

    return false;
}

bool GroupsAction::isFactoryResettable(bool recursive /*= true*/) const
{
    for (auto groupAction : _groupActions)
        if (groupAction->isFactoryResettable(recursive))
            return true;

    return false;
}

void GroupsAction::reset(bool recursive /*= true*/)
{
    for (auto groupAction : _groupActions)
        groupAction->reset(recursive);
}

GroupsAction::Widget::Widget(QWidget* parent, GroupsAction* groupsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupsAction, widgetFlags),
    _groupsAction(groupsAction),
    _layout(),
    _filteredActionsAction(this, true),
    _toolbarWidget(parent),
    _toolbarLayout(),
    _filterAction(this, "Search"),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _treeWidget()
{
    // Configure layout
    _layout.setMargin(0);
    _layout.setAlignment(Qt::AlignTop);

    // Apply layout to widget
    setLayout(&_layout);

    // Create UI
    createToolbar(widgetFlags);
    createTreeWidget(widgetFlags);

    // Add filtering action to the groups action and hide by default
    _groupsAction->addGroupAction(&_filteredActionsAction, false);

    // Perform an initial update of the toolbar and action filtering
    updateToolbar();
    updateFiltering();
}

void GroupsAction::Widget::createToolbar(const std::int32_t& widgetFlags)
{
    // Filter action is in search mode
    _filterAction.setSearchMode(true);
    _filterAction.setPlaceHolderString("Filter properties by name...");

    // Set action icon
    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));

    // Set action tooltips
    _filterAction.setToolTip("Filter properties by name");
    _expandAllAction.setToolTip("Expand all property sections");
    _collapseAllAction.setToolTip("Collapse all property sections");

    // Configure toolbar layout
    _toolbarLayout.setContentsMargins(0, 2, 0, 2);
    _toolbarLayout.setSpacing(4);

    // Add toolbar items
    if (widgetFlags & Filtering)
        _toolbarLayout.addWidget(_filterAction.createWidget(this), 2);

    if (widgetFlags & Expansion) {
        if (widgetFlags & Filtering)
            _toolbarLayout.addWidget(createVerticalDivider());

        _toolbarLayout.addWidget(_expandAllAction.createWidget(this, TriggerAction::Icon));
        _toolbarLayout.addWidget(_collapseAllAction.createWidget(this, TriggerAction::Icon));
    }

    // Set toolbar widget layout
    _toolbarWidget.setLayout(&_toolbarLayout);

    // And add to the main layout
    _layout.addWidget(&_toolbarWidget);

    // Update toolbar when a group action is expanded/collapsed/added/removed/shown/hidden
    connect(_groupsAction, &GroupsAction::groupActionExpanded, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionCollapsed, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionAdded, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionRemoved, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionShown, this, &Widget::updateToolbar);
    connect(_groupsAction, &GroupsAction::groupActionHidden, this, &Widget::updateToolbar);

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

    // Add/remove group section tree items when requested
    connect(_groupsAction, &GroupsAction::groupActionAdded, this, &Widget::addGroupAction);
    connect(_groupsAction, &GroupsAction::groupActionRemoved, this, &Widget::removeGroupAction);

    // Show/hide group section tree items when requested
    connect(_groupsAction, &GroupsAction::groupActionShown, this, &Widget::showGroupAction);
    connect(_groupsAction, &GroupsAction::groupActionHidden, this, &Widget::hideGroupAction);
}

void GroupsAction::Widget::updateToolbar()
{
    // Set toolbar read-only status based on whether a dataset is loaded
    _toolbarWidget.setEnabled(_groupsAction->getGroupActions().count() > 1);

    // Set read-only states of actions
    _expandAllAction.setEnabled(_groupsAction->canExpandAll());
    _collapseAllAction.setEnabled(_groupsAction->canCollapseAll());
}

void GroupsAction::Widget::updateFiltering()
{
#ifdef _VERBOSE
    qDebug() << "Updating action filtering";
#endif

    // Get filter string
    const auto filterString = _filterAction.getString();

    // Establish whether we are filtering out actions or not
    const auto areFiltering = !filterString.isEmpty();

    // Get group actions
    auto groupActions = _groupsAction->getGroupActions();

    // Set actions visibility
    for (auto groupAction : groupActions) {
        if (groupAction == groupActions.first())
            _groupsAction->setGroupActionVisibility(groupAction, areFiltering);
        else
            _groupsAction->setGroupActionVisibility(groupAction, !areFiltering);
    }

    // Do not include actions from the special filtering group action
    if (!groupActions.isEmpty())
        groupActions.removeFirst();

    // Found child widget actions
    QVector<WidgetAction*> foundActions;

    for (auto groupAction : groupActions)
        foundActions << groupAction->findChildren(filterString, false);

    // Update filtered actions group action
    _filteredActionsAction.setExpanded(true);
    _filteredActionsAction.setActions(foundActions);
    _filteredActionsAction.setText(foundActions.count() == 0 ? "No properties found" : QString("Found %1 proper%2").arg(QString::number(foundActions.count()), foundActions.count() == 1 ? "ty": "ties"));
}

void GroupsAction::Widget::addGroupAction(GroupAction* groupAction)
{
#ifdef _VERBOSE
    qDebug().noquote() << QString("Add %1 to tree widget").arg(groupAction->getSettingsPath());
#endif

    // Create new tree widget item for the section expand/collapse button
    _groupSectionTreeItems[groupAction] = new GroupSectionTreeItem(&_treeWidget, groupAction);

    // Set expansion state from group action
    _groupSectionTreeItems[groupAction]->setExpanded(groupAction->isExpanded());
    _groupSectionTreeItems[groupAction]->setHidden(_groupsAction->isGroupActionHidden(groupAction));

    update();
}

void GroupsAction::Widget::removeGroupAction(GroupAction* groupAction)
{
#ifdef _VERBOSE
    qDebug().noquote() << QString("Remove %1 from tree widget").arg(groupAction->getSettingsPath());
#endif

    Q_ASSERT(_groupSectionTreeItems.contains(groupAction));

    if (!_groupSectionTreeItems.contains(groupAction))
        return;

    // Get pointer to top-level group section tree item
    const auto groupSectionTreeItem = _groupSectionTreeItems[groupAction];

    // Remove the item from the tree widget
    _treeWidget.removeItemWidget(groupSectionTreeItem, 0);

    // Delete it physically
    delete _groupSectionTreeItems[groupAction];

    // And remove the group section tree item from the map
    _groupSectionTreeItems.remove(groupAction);
}

void GroupsAction::Widget::showGroupAction(GroupAction* groupAction)
{
    Q_ASSERT(_groupSectionTreeItems.contains(groupAction));

    _groupSectionTreeItems[groupAction]->setHidden(_groupsAction->isGroupActionHidden(groupAction));
}

void GroupsAction::Widget::hideGroupAction(GroupAction* groupAction)
{
    Q_ASSERT(_groupSectionTreeItems.contains(groupAction));

    _groupSectionTreeItems[groupAction]->setHidden(_groupsAction->isGroupActionHidden(groupAction));
}

}
}
