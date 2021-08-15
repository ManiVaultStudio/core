#include "PointsInfoAction.h"
#include "DataHierarchyItem.h"
#include "PointData.h"

#include <QGridLayout>
#include <QListView>
#include <QAbstractEventDispatcher>

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
    _numberOfSelectedPointsAction(this, "Number of selected points"),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update"),
    _selectionChangedTimer()
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

    _selectionChangedTimer.setSingleShot(true);

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
        auto& points = _dataHierarchyItem->getDataset<Points>();

        _numberOfPointsAction.setString(QString::number(points.getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(points.getNumDimensions()));
        _memorySizeAction.setString(getNoBytesHumanReadable(points.getNumPoints() * points.getNumDimensions() * 4));
        _numberOfSelectedPointsAction.setString(QString::number(getSelectedIndices().size()));
    };

    registerDataEventByType(PointType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
            {
                if (dataEvent->getType() == EventType::SelectionChanged)
                    _selectionChangedTimer.start(100);

                updateActions();

                break;
            }
            
            default:
                break;
        }
    });

    updateActions();

    connect(&_selectionChangedTimer, &QTimer::timeout, this, [this]() {
        if (_manualUpdateAction.isChecked())
            return;

        emit selectedIndicesChanged(getSelectedIndices());
    });

    const auto updateUpdateAction = [this]() -> void {
        _updateAction.setEnabled(_manualUpdateAction.isChecked());
    };

    connect(&_manualUpdateAction, &ToggleAction::toggled, this, [this, updateUpdateAction]() {
        updateUpdateAction();
    });

    connect(&_updateAction, &TriggerAction::triggered, this, [this]() {
        emit selectedIndicesChanged(getSelectedIndices());
    });

    updateUpdateAction();
}

const std::vector<std::uint32_t>& PointsInfoAction::getSelectedIndices() const
{
    auto& points    = _dataHierarchyItem->getDataset<Points>();
    auto& selection = dynamic_cast<Points&>(points.getSelection());

    return selection.indices;
}

const std::vector<QString>& PointsInfoAction::getDimensionNames() const
{
    return _dataHierarchyItem->getDataset<Points>().getDimensionNames();
}

PointsInfoAction::Widget::Widget(QWidget* parent, PointsInfoAction* pointsInfoAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionGroup::FormWidget(parent, pointsInfoAction)
{
    addWidgetAction(pointsInfoAction->getNumberOfPointsAction());
    addWidgetAction(pointsInfoAction->getNumberOfDimensionsAction());
    addWidgetAction(pointsInfoAction->getMemorySizeAction());
    addWidgetAction(pointsInfoAction->getNumberOfSelectedPointsAction());

    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(100);
    //selectedIndicesListWidget->setSpacing(1);

    auto numberOfRows = layout()->rowCount();

    layout()->addWidget(new QLabel("Selected indices:"), numberOfRows, 0);
    
    auto selectedIndicesLayout = new QHBoxLayout();

    selectedIndicesLayout->setMargin(0);
    selectedIndicesLayout->addWidget(selectedIndicesListWidget);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(pointsInfoAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(pointsInfoAction->getManualUpdateAction().createCheckBoxWidget(this));
    updateLayout->addStretch(1);

    selectedIndicesLayout->addLayout(updateLayout);

    layout()->addLayout(selectedIndicesLayout, numberOfRows, 1);

    numberOfRows = layout()->rowCount();

    auto dimensionNamesListWidget = new QListView();

    dimensionNamesListWidget->setFixedHeight(100);

    layout()->addWidget(new QLabel("Dimension names:"), numberOfRows, 0);
    layout()->addWidget(dimensionNamesListWidget, numberOfRows, 1);

    const auto updateSelectedIndicesWidget = [this, pointsInfoAction, selectedIndicesListWidget]() -> void {
        QStringList items;

        for (auto selectedIndex : pointsInfoAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListWidget->setModel(new QStringListModel(items));
    };

    const auto updateDimensionNamesWidget = [this, pointsInfoAction, dimensionNamesListWidget]() -> void {
        QStringList items;

        for (auto dimensionName : pointsInfoAction->getDimensionNames())
            items << dimensionName;

        dimensionNamesListWidget->setModel(new QStringListModel(items));
    };

    connect(pointsInfoAction, &PointsInfoAction::selectedIndicesChanged, this, [this, updateSelectedIndicesWidget]() {
        updateSelectedIndicesWidget();
    });

    updateSelectedIndicesWidget();
    updateDimensionNamesWidget();
}
