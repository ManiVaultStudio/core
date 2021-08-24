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

using namespace hdps::gui;

/**
 * Points info action class
 *
 * Action class for displaying basic points info
 *
 * @author Thomas Kroes
 */
class PointsInfoAction : public GroupAction, public hdps::EventListener
{
    Q_OBJECT

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
    StringAction                _numberOfPointsAction;              /** Number of points action */
    StringAction                _numberOfDimensionsAction;          /** Number of dimensions action */
    StringAction                _memorySizeAction;                  /** Memory size action */
    StringAction                _numberOfSelectedPointsAction;      /** Memory size action */
    SelectedIndicesAction       _selectedIndicesAction;             /** Selected indices action */
    DimensionNamesAction        _dimensionNamesAction;              /** Dimension names action */
};