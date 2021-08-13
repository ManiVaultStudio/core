#include "PointsInfoAction.h"
#include "DataHierarchyItem.h"
#include "PointData.h"

using namespace hdps;
using namespace hdps::gui;

PointsInfoAction::PointsInfoAction(CoreInterface* core, const QString& datasetName, QObject* parent /*= nullptr*/) :
    WidgetActionGroup(parent, false),
    EventListener(),
    _core(core),
    _dataHierarchyItem(nullptr),
    _numberOfPointsAction(this, "Number of points"),
    _numberOfDimensionsAction(this, "Number of dimensions"),
    _memorySizeAction(this, "Occupied memory"),
    _numberOfSelectedPointsAction(this, "Number of selected points")
{
    setText("Info");
    setEventCore(_core);

    _dataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    _numberOfPointsAction.setEnabled(false);
    _numberOfDimensionsAction.setEnabled(false);
    _memorySizeAction.setEnabled(false);
    _numberOfSelectedPointsAction.setEnabled(false);

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
        auto& points    = _dataHierarchyItem->getDataset<Points>();
        auto& selection = dynamic_cast<Points&>(points.getSelection());

        _numberOfPointsAction.setString(QString::number(points.getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(points.getNumDimensions()));
        _memorySizeAction.setString(getNoBytesHumanReadable(points.getNumPoints() * points.getNumDimensions() * 4));
        _numberOfSelectedPointsAction.setString(QString::number(selection.indices.size()));
    };

    registerDataEventByType(PointType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
                updateActions();
        }
    });

    updateActions();
}

PointsInfoAction::Widget::Widget(QWidget* parent, PointsInfoAction* pointsInfoAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionGroup::FormWidget(parent, pointsInfoAction)
{
    addWidgetAction(pointsInfoAction->getNumberOfPointsAction());
    addWidgetAction(pointsInfoAction->getNumberOfDimensionsAction());
    addWidgetAction(pointsInfoAction->getMemorySizeAction());
    addWidgetAction(pointsInfoAction->getNumberOfSelectedPointsAction());
}
