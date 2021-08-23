#include "DimensionNamesAction.h"

#include "PointData.h"

#include <QHBoxLayout>
#include <QListView>

using namespace hdps;

DimensionNamesAction::DimensionNamesAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName) :
    WidgetAction(parent),
    EventListener(),
    _dataHierarchyItem(core->getDataHierarchyItem(datasetName)),
    _dimensionNames()
{
    setText("Dimension names");
    setEventCore(core);

    const auto updateDimensionNames = [this]() -> void {
        _dimensionNames.clear();

        auto& points = _dataHierarchyItem->getDataset<Points>();

        auto& dimensionNames = points.getDimensionNames();

        if (dimensionNames.empty()) {
            for (std::uint32_t dimensionId = 0; dimensionId < points.getNumDimensions(); dimensionId++)
                _dimensionNames << QString("Dim %1").arg(dimensionId);
        }
        else {
            for (auto dimensionName : dimensionNames)
                _dimensionNames << dimensionName;
        }
    };

    registerDataEventByType(PointType, [this, updateDimensionNames](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
            {
                updateDimensionNames();
                emit dimensionNamesChanged(_dimensionNames);
                break;
            }

            default:
                break;
        }
    });

    updateDimensionNames();
}

QStringList DimensionNamesAction::getDimensionNames() const
{
    return _dimensionNames;
}

DimensionNamesAction::Widget::Widget(QWidget* parent, DimensionNamesAction* dimensionNamesAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, dimensionNamesAction, state)
{
    auto layout = new QHBoxLayout();

    auto listView = new QListView();

    listView->setFixedHeight(100);

    layout->setMargin(0);
    layout->addWidget(listView);

    const auto updateListView = [this, dimensionNamesAction, listView]() -> void {
        QStringList items;

        for (auto dimensionName : dimensionNamesAction->getDimensionNames())
            items << dimensionName;

        listView->setModel(new QStringListModel(items));
    };

    connect(dimensionNamesAction, &DimensionNamesAction::dimensionNamesChanged, this, [this, updateListView](const QStringList& dimensionNames) {
        updateListView();
    });

    updateListView();

    setLayout(layout);
}
