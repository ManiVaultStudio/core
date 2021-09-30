#include "InfoAction.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, CoreInterface* core, const QString& datasetName) :
    GroupAction(parent, false),
    EventListener(),
    _core(core),
    _points(datasetName),
    _numberOfPointsAction(this, "Number of points"),
    _numberOfDimensionsAction(this, "Number of dimensions"),
    _memorySizeAction(this, "Occupied memory"),
    _numberOfSelectedPointsAction(this, "Number of selected points"),
    _selectedIndicesAction(this, core, datasetName),
    _dimensionNamesAction(this, core, datasetName)
{
    setText("Info");
    setEventCore(_core);

    _numberOfPointsAction.setEnabled(false);
    _numberOfDimensionsAction.setEnabled(false);
    _memorySizeAction.setEnabled(false);
    _numberOfSelectedPointsAction.setEnabled(false);

    _numberOfPointsAction.setMayReset(false);
    _numberOfDimensionsAction.setMayReset(false);
    _memorySizeAction.setMayReset(false);
    _numberOfSelectedPointsAction.setMayReset(false);
    _selectedIndicesAction.setMayReset(false);
    _dimensionNamesAction.setMayReset(false);

    _numberOfPointsAction.setToolTip("The number of points");
    _numberOfDimensionsAction.setToolTip("The number of dimensions in the point data");
    _memorySizeAction.setToolTip("The amount of memory occupied by the dataset");
    _numberOfSelectedPointsAction.setToolTip("The number of selected points in the dataset");

    const auto getNoBytesHumanReadable = [](std::uint32_t noBytes) -> QString
    {
        QStringList list{ "KB", "MB", "GB", "TB" };

        QStringListIterator it(list);
        QString unit("bytes");

        while (noBytes >= 1024.0 && it.hasNext())
        {
            unit = it.next();
            noBytes /= 1024.0;
        }

        return QString::number(noBytes, 'f', 2) + " " + unit;
    };

    const auto updateActions = [this, getNoBytesHumanReadable]() -> void {
        if (!_points.isValid())
            return;

        auto& selection = dynamic_cast<Points&>(_points->getSelection());

        _numberOfPointsAction.setString(QString::number(_points->getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(_points->getNumDimensions()));
        _memorySizeAction.setString(getNoBytesHumanReadable(_points->getNumPoints() * _points->getNumDimensions() * 4));
        _numberOfSelectedPointsAction.setString(QString::number(selection.indices.size()));
    };

    registerDataEventByType(PointType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_points.isValid())
            return;

        if (dataEvent->dataSetName != _points->getName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
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
