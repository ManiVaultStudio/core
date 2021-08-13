#include "PointsInfoAction.h"
#include "DataHierarchyItem.h"
#include "PointData.h"

#include <QGridLayout>
#include <QListView>

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

    const auto getSelectedIndices = [this]() -> std::vector<std::uint32_t> {
        auto& points    = _dataHierarchyItem->getDataset<Points>();
        auto& selection = dynamic_cast<Points&>(points.getSelection());

        return selection.indices;
    };

    const auto updateActions = [this, getNoBytesHumanReadable]() -> void {
        auto& points = _dataHierarchyItem->getDataset<Points>();

        _numberOfPointsAction.setString(QString::number(points.getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(points.getNumDimensions()));
        _memorySizeAction.setString(getNoBytesHumanReadable(points.getNumPoints() * points.getNumDimensions() * 4));
        _numberOfSelectedPointsAction.setString(QString::number(_selectedIndices.size()));
    };

    registerDataEventByType(PointType, [this, updateActions, getSelectedIndices](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
            {
                if (dataEvent->getType() == EventType::SelectionChanged) {
                    _selectedIndices = getSelectedIndices();
                    emit selectedIndicesChanged(_selectedIndices);
                }

                updateActions();

                break;
            }
            
            default:
                break;
        }
    });

    updateActions();
}

const std::vector<std::uint32_t>& PointsInfoAction::getSelectedIndices() const
{
    return _selectedIndices;
}

PointsInfoAction::Widget::Widget(QWidget* parent, PointsInfoAction* pointsInfoAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionGroup::FormWidget(parent, pointsInfoAction)
{
    addWidgetAction(pointsInfoAction->getNumberOfPointsAction());
    addWidgetAction(pointsInfoAction->getNumberOfDimensionsAction());
    addWidgetAction(pointsInfoAction->getMemorySizeAction());
    addWidgetAction(pointsInfoAction->getNumberOfSelectedPointsAction());

    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(200);

    layout()->addWidget(new QLabel("Selected indices:"), 5, 0);
    layout()->addWidget(selectedIndicesListWidget, 5, 1);

    const auto updateSelectedIndicesWidget = [this, pointsInfoAction, selectedIndicesListWidget]() -> void {
        QStringList items;

        for (auto selectedIndex : pointsInfoAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListWidget->setModel(new QStringListModel(items));
    };

    connect(pointsInfoAction, &PointsInfoAction::selectedIndicesChanged, this, [this, updateSelectedIndicesWidget]() {
        updateSelectedIndicesWidget();
    });

    updateSelectedIndicesWidget();
}
