#include "InfoAction.h"

#include "util/Miscellaneous.h"
#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

InfoAction::InfoAction(QObject* parent, CoreInterface* core, Points& points) :
    GroupAction(parent, true),
    _core(core),
    _points(&points),
    _numberOfPointsAction(this, "Number of points"),
    _numberOfDimensionsAction(this, "Number of dimensions"),
    _memorySizeAction(this, "Occupied memory"),
    _numberOfSelectedPointsAction(this, "Number of selected points"),
    _selectedIndicesAction(this, core, points),
    _dimensionNamesAction(this, core, points)
{
    setText("Info");

    _numberOfPointsAction.setEnabled(false);
    _numberOfDimensionsAction.setEnabled(false);
    _memorySizeAction.setEnabled(false);
    _numberOfSelectedPointsAction.setEnabled(false);

    _numberOfPointsAction.setToolTip("The number of points");
    _numberOfDimensionsAction.setToolTip("The number of dimensions in the point data");
    _memorySizeAction.setToolTip("The amount of memory occupied by the dataset");
    _numberOfSelectedPointsAction.setToolTip("The number of selected points in the dataset");

    const auto updateActions = [this]() -> void {
        if (!_points.isValid())
            return;

        auto& selectedIndices = _selectedIndicesAction.getSelectedIndices();

        _numberOfPointsAction.setString(QString::number(_points->getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(_points->getNumDimensions()));
        _memorySizeAction.setString(getNoBytesHumanReadable(_points->getNumPoints() * _points->getNumDimensions() * 4));
        _numberOfSelectedPointsAction.setString(QString::number(selectedIndices.size()));
    };

    connect(&_selectedIndicesAction, &SelectedIndicesAction::selectedIndicesChanged, this, updateActions);

    _eventListener.setEventCore(Application::core());
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
    _eventListener.registerDataEventByType(PointType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_points.isValid())
            return;

        if (dataEvent->getDataset() != _points)
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::DataSelectionChanged:
            {
                updateActions();
                break;
            }
            
            default:
                break;
        }
    });

    updateActions();
}
