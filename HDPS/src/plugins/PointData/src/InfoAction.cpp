#include "InfoAction.h"

#include "util/Miscellaneous.h"
#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

InfoAction::InfoAction(QObject* parent, const Dataset<Points>& points) :
    GroupAction(parent, true),
    _points(points),
    _dataStorageAction(this, "Storage type"),
    _proxyDatasetsAction(this, points),
    _numberOfPointsAction(this, "Number of points"),
    _numberOfDimensionsAction(this, "Number of dimensions"),
    _rawDataSizeAction(this, "Raw data size"),
    _numberOfSelectedPointsAction(this, "Number of selected points"),
    _selectedIndicesAction(this, points),
    _dimensionNamesAction(this, points)
{
    setText("Info");

    _dataStorageAction.setEnabled(false);
    _numberOfPointsAction.setEnabled(false);
    _numberOfDimensionsAction.setEnabled(false);
    _rawDataSizeAction.setEnabled(false);
    _numberOfSelectedPointsAction.setEnabled(false);

    _dataStorageAction.setToolTip("The type of data storage (e.g. owner or proxy)");
    _proxyDatasetsAction.setToolTip("Proxy datasets");
    _numberOfPointsAction.setToolTip("The number of points");
    _numberOfDimensionsAction.setToolTip("The number of dimensions in the point data");
    _rawDataSizeAction.setToolTip("The amount of memory occupied for raw data by the dataset");
    _numberOfSelectedPointsAction.setToolTip("The number of selected points in the dataset");

    const auto updateActions = [this]() -> void {
        if (!_points.isValid())
            return;

        auto& selectedIndices = _selectedIndicesAction.getSelectedIndices();

        _dataStorageAction.setString(_points->isProxy() ? "Proxy" : "Owner");
        _numberOfPointsAction.setString(QString::number(_points->getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(_points->getNumDimensions()));
        _rawDataSizeAction.setString(_points->getRawDataSizeHumanReadable());
        _numberOfSelectedPointsAction.setString(QString::number(selectedIndices.size()));
    };

    connect(&_selectedIndicesAction, &SelectedIndicesAction::selectedIndicesChanged, this, updateActions);

    connect(&_points, &Dataset<Points>::dataChanged, this, updateActions);
    connect(&_points, &Dataset<Points>::dataSelectionChanged, this, updateActions);

    updateActions();
}
