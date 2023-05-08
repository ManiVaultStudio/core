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

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param actionsModel Reference to actions input model
     */
    ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel);

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    HierarchyWidget& getHierarchyWidget();

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
    AbstractActionsModel&   _actionsModel;      /** Input actions model */
    ActionsFilterModel      _filterModel;       /** Hierarchical actions filter model */
    HierarchyWidget         _hierarchyWidget;   /** Widget for displaying action hierarchy */
};

}
