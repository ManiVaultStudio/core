#pragma once

#include "models/AbstractActionsModel.h"
#include "models/ActionsFilterModel.h"
#include "widgets/HierarchyWidget.h"

#include <QWidget>

namespace hdps::gui
{

/**
 * Actions widget class
 *
 * Widget class for viewing the contents of an actions model (e.g. list or hierarchy)
 *
 * @author Thomas Kroes
 */
class ActionsWidget final : public QWidget
{
    Q_OBJECT

    using RequestContextMenuFN = std::function<void(QMenu* menu, WidgetActions selectedActions)>;

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param actionsModel Reference to actions input model
     * @param itemTypeName Name of the action in the hierarchy widget
     */
    ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel, const QString& itemTypeName = "Parameter");

    /**
     * Get filter model
     * @return Reference to actions filter model
     */
    ActionsFilterModel& getFilterModel();

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    HierarchyWidget& getHierarchyWidget();

    /**
     * Sets the callback function which is called when the widget asks for a context menu
     * @param requestContextMenu Callback function which is called when the widget asks for a context menu
     */
    void setRequestContextMenuCallback(RequestContextMenuFN requestContextMenu);

private:
    
    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Resize sections based on content */
    void resizeSectionsToContent();

    /**
     * Highlight all items in the \p selection
     * @param selection Item selection
     * @param highlight Whether to highlight or not
     */
    void highlightSelection(const QItemSelection& selection, bool highlight);

private:
    AbstractActionsModel&   _actionsModel;              /** Input actions model */
    ActionsFilterModel      _filterModel;               /** Hierarchical actions filter model */
    HierarchyWidget         _hierarchyWidget;           /** Widget for displaying action hierarchy */
    RequestContextMenuFN    _requestContextMenu;        /** Callback which is called when a context menu is requested */
};

}