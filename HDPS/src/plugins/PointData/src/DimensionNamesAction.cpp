#include "DimensionNamesAction.h"

#include "event/Event.h"

#include <QHBoxLayout>
#include <QListView>

using namespace hdps;

DimensionNamesAction::DimensionNamesAction(QObject* parent, hdps::CoreInterface* core, Points& points) :
    WidgetAction(parent),
    EventListener(),
    _points(&points),
    _dimensionNames(),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update")
{
    setText("Dimension names");
    setEventCore(core);

    _updateAction.setToolTip("Update the dimension names");
    _manualUpdateAction.setToolTip("Update the dimension names manually");

    const auto updateDimensionNames = [this]() -> void {
        if (!_points.isValid())
            return;

        _dimensionNames.clear();

        auto& dimensionNames = _points->getDimensionNames();

        if (dimensionNames.empty()) {
            for (std::uint32_t dimensionId = 0; dimensionId < _points->getNumDimensions(); dimensionId++)
                _dimensionNames << QString("Dim %1").arg(dimensionId);
        }
        else {
            for (auto dimensionName : dimensionNames)
                _dimensionNames << dimensionName;
        }
    };

    const auto dataChanged = [this]() -> void {
        _manualUpdateAction.setChecked(_points->getNumDimensions() > 1000);

        if (_manualUpdateAction.isChecked())
            return;

        _updateAction.trigger();
    };

    registerDataEventByType(PointType, [this, dataChanged](hdps::DataEvent* dataEvent) {
        if (!_points.isValid())
            return;

        if (dataEvent->getDataset() != _points)
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            {
                dataChanged();
                break;
            }

            default:
                break;
        }
    });

    dataChanged();

    const auto updateUpdateAction = [this]() -> void {
        _updateAction.setEnabled(_manualUpdateAction.isChecked());
    };

    connect(&_manualUpdateAction, &ToggleAction::toggled, this, [this, updateUpdateAction]() {
        updateUpdateAction();
    });

    connect(&_updateAction, &TriggerAction::triggered, this, [this, updateDimensionNames]() {
        updateDimensionNames();
        emit dimensionNamesChanged(_dimensionNames);
    });

    updateUpdateAction();
}

QStringList DimensionNamesAction::getDimensionNames() const
{
    return _dimensionNames;
}

DimensionNamesAction::Widget::Widget(QWidget* parent, DimensionNamesAction* dimensionNamesAction) :
    WidgetActionWidget(parent, dimensionNamesAction)
{
    auto layout     = new QHBoxLayout();
    auto listView   = new QListView();

    listView->setFixedHeight(100);

    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(listView);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(dimensionNamesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(dimensionNamesAction->getManualUpdateAction().createWidget(this));
    updateLayout->addStretch(1);

    layout->addLayout(updateLayout);

    setLayout(layout);

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
}
