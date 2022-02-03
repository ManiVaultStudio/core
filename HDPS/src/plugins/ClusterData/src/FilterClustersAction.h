#pragma once

#include <actions/WidgetAction.h>
#include <actions/StringAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

/**
 * Filter clusters action class
 *
 * Action class for filtering clusters
 *
 * @author Thomas Kroes
 */
class FilterClustersAction : public WidgetAction
{
protected:

    /** Widget class for filter clusters action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param filterClustersAction Pointer to filter clusters action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, FilterClustersAction* filterClustersAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the filter clusters action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    FilterClustersAction(ClustersActionWidget* clustersActionWidget);

public: // Action getters

    StringAction& getNameFilterAction() { return _nameFilterAction; }

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
    StringAction            _nameFilterAction;          /** Name filter action */
};
