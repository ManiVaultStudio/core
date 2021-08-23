#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include "SelectedIndicesAction.h"
#include "DimensionNamesAction.h"

#include <QTimer>

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

/**
 * Points info action class
 *
 * Action class for displaying basic points info
 *
 * @author Thomas Kroes
 */
class PointsInfoAction : public hdps::gui::WidgetActionGroup, public hdps::EventListener
{
    Q_OBJECT

protected:

    /** Widget class for points info action */
    class Widget : public hdps::gui::WidgetActionGroup::FormWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param infoAction Pointer to points info action
         * @param state State of the widget
         */
        Widget(QWidget* parent, PointsInfoAction* pointsInfoAction, const hdps::gui::WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the info action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const hdps::gui::WidgetActionWidget::State& state = hdps::gui::WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     * @param parent Pointer to parent object
     */
    PointsInfoAction(hdps::CoreInterface* core, const QString& datasetName, QObject* parent = nullptr);

public: // Action getters

    hdps::gui::StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    hdps::gui::StringAction& getNumberOfDimensionsAction() { return _numberOfDimensionsAction; }
    hdps::gui::StringAction& getMemorySizeAction() { return _memorySizeAction; }
    hdps::gui::StringAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }
    SelectedIndicesAction& getSelectedIndicesAction() { return _selectedIndicesAction; }
    DimensionNamesAction& getDimensionNamesAction() { return _dimensionNamesAction; }

protected:
    hdps::CoreInterface*        _core;                              /** Pointer to the core */
    hdps::DataHierarchyItem*    _dataHierarchyItem;                 /** Pointer to the data hierarchy item of the points dataset */
    hdps::gui::StringAction     _numberOfPointsAction;              /** Number of points action */
    hdps::gui::StringAction     _numberOfDimensionsAction;          /** Number of dimensions action */
    hdps::gui::StringAction     _memorySizeAction;                  /** Memory size action */
    hdps::gui::StringAction     _numberOfSelectedPointsAction;      /** Memory size action */
    SelectedIndicesAction       _selectedIndicesAction;             /** Selected indices action */
    DimensionNamesAction        _dimensionNamesAction;              /** Dimension names action */
};