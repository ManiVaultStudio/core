#pragma once

#include "PointData.h"

#include "SelectedIndicesAction.h"
#include "DimensionNamesAction.h"

#include <actions/StringAction.h>

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
     * @param points Smart pointer to points dataset
     */
    InfoAction(QObject* parent, const Dataset<Points>& points);

public: // Action getters

    StringAction& getDataStorageAction() { return _dataStorageAction; }
    StringAction& getNumberOfProxyDatasetsAction() { return _numberOfProxyDatasetsAction; }
    StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    StringAction& getNumberOfDimensionsAction() { return _numberOfDimensionsAction; }
    StringAction& getMemorySizeAction() { return _memorySizeAction; }
    StringAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }
    SelectedIndicesAction& getSelectedIndicesAction() { return _selectedIndicesAction; }
    DimensionNamesAction& getDimensionNamesAction() { return _dimensionNamesAction; }

protected:
    Dataset<Points>         _points;                            /** Points dataset reference */
    StringAction            _dataStorageAction;                 /** Type of data storage action */
    StringAction            _numberOfProxyDatasetsAction;       /** Number of points action */
    StringAction            _numberOfPointsAction;              /** Number of points action */
    StringAction            _numberOfDimensionsAction;          /** Number of dimensions action */
    StringAction            _memorySizeAction;                  /** Memory size action */
    StringAction            _numberOfSelectedPointsAction;      /** Number of selected points action */
    SelectedIndicesAction   _selectedIndicesAction;             /** Selected indices action */
    DimensionNamesAction    _dimensionNamesAction;              /** Dimension names action */
};