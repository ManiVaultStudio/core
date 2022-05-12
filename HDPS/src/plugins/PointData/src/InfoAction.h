#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include "SelectedIndicesAction.h"
#include "DimensionNamesAction.h"

#include "PointData.h"

#include <QTimer>

namespace hdps {
    class CoreInterface;
}

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

/**
 * Info action class
 *
 * Action class for displaying basic points info
 *
 * @author Thomas Kroes
 */
class InfoAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param points Reference to points dataset
     */
    InfoAction(QObject* parent, CoreInterface* core, Points& points);

public: // Action getters

    StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    StringAction& getNumberOfDimensionsAction() { return _numberOfDimensionsAction; }
    StringAction& getMemorySizeAction() { return _memorySizeAction; }
    StringAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }
    SelectedIndicesAction& getSelectedIndicesAction() { return _selectedIndicesAction; }
    DimensionNamesAction& getDimensionNamesAction() { return _dimensionNamesAction; }

protected:
    CoreInterface*          _core;                              /** Pointer to the core */
    Dataset<Points>         _points;                            /** Points dataset reference */
    StringAction            _numberOfPointsAction;              /** Number of points action */
    StringAction            _numberOfDimensionsAction;          /** Number of dimensions action */
    StringAction            _memorySizeAction;                  /** Memory size action */
    StringAction            _numberOfSelectedPointsAction;      /** Number of selected points action */
    SelectedIndicesAction   _selectedIndicesAction;             /** Selected indices action */
    DimensionNamesAction    _dimensionNamesAction;              /** Dimension names action */
    hdps::EventListener     _eventListener;                     /** Listen to HDPS events */
};