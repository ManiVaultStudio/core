#pragma once

#include <actions/Actions.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersFilterModel;
class QItemSelectionModel;

/**
 * Select clusters action class
 *
 * Action class for selection clusters (all/none/invert)
 *
 * @author Thomas Kroes
 */
class SelectClustersAction : public WidgetAction
{
public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        SelectAll       = 0x00001,      /** Includes select all UI */
        SelectNone      = 0x00002,      /** Includes select none UI */
        SelectInvert    = 0x00004,      /** Includes select invert UI */

        Default = SelectAll | SelectNone | SelectInvert
    };

protected:

    /** Widget class for select clusters action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param selectClustersAction Pointer to select clusters action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, SelectClustersAction* selectClustersAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the select clusters action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
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
    SelectClustersAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

public: // Action getters

    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getSelectNoneAction() { return _selectNoneAction; }
    TriggerAction& getSelectInvertAction() { return _selectInvertAction; }

protected:
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to selection model */
    TriggerAction           _selectAllAction;       /** Select all clusters action */
    TriggerAction           _selectNoneAction;      /** Clear cluster selection action */
    TriggerAction           _selectInvertAction;    /** Invert the cluster selection action */
};
