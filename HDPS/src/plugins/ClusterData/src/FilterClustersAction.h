#pragma once

#include <actions/WidgetAction.h>
#include <actions/StringAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersFilterModel;
class QItemSelectionModel;

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
     * @param parent Pointer to parent object
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    FilterClustersAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

public: // Action getters

    StringAction& getNameFilterAction() { return _nameFilterAction; }

protected:
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to selection model */
    StringAction            _nameFilterAction;      /** Name filter action */
};
