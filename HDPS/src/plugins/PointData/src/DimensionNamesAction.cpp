#include "DimensionNamesAction.h"

#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>

using namespace hdps;

DimensionNamesAction::DimensionNamesAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent),
    _points(points),
    _dimensionNames(),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update")
{
    setText("Dimension names");
}

hdps::Dataset<Points>& DimensionNamesAction::getPoints()
{
    return _points;
}

QStringList DimensionNamesAction::getDimensionNames() const
{
    if (!_points.isValid())
        return QStringList();

    QStringList dimensionNames;

    if (_points->getDimensionNames().empty()) {
        for (std::uint32_t dimensionId = 0; dimensionId < _points->getNumDimensions(); dimensionId++)
            dimensionNames << QString("Dim %1").arg(dimensionId);
    }
    else {
        for (auto dimensionName : _points->getDimensionNames())
            dimensionNames << dimensionName;
    }

    return dimensionNames;
}

DimensionNamesAction::Widget::Widget(QWidget* parent, DimensionNamesAction* dimensionNamesAction) :
    WidgetActionWidget(parent, dimensionNamesAction),
    _timer(),
    _dirty(false)
{
    auto layout     = new QHBoxLayout();
    auto listView   = new QListView();

    listView->setFixedHeight(100);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(listView);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(dimensionNamesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(dimensionNamesAction->getManualUpdateAction().createWidget(this));
    updateLayout->addStretch(1);

    layout->addLayout(updateLayout);

    setLayout(layout);

    _timer.setSingleShot(true);

    const auto updateListView = [this, dimensionNamesAction, listView]() -> void {
        listView->setModel(new QStringListModel(dimensionNamesAction->getDimensionNames()));
    };

    const auto updateActions = [this, dimensionNamesAction]() -> void {
        dimensionNamesAction->getUpdateAction().setEnabled(dimensionNamesAction->getManualUpdateAction().isChecked() && _dirty);
    };

    connect(&dimensionNamesAction->getUpdateAction(), &TriggerAction::triggered, this, [this, updateListView, updateActions]() -> void {
        updateListView();

        _dirty = false;

        updateActions();
    });

    connect(&dimensionNamesAction->getManualUpdateAction(), &TriggerAction::triggered, this, updateActions);

    connect(&_timer, &QTimer::timeout, this, [this, updateListView]() -> void {
        if (_timer.isActive())
            _timer.start(LAZY_UPDATE_INTERVAL);
        else {
            _timer.stop();
            updateListView();
        }
    });

    connect(&dimensionNamesAction->getPoints(), &Dataset<Points>::dataChanged, this, [this, dimensionNamesAction]() -> void {
        dimensionNamesAction->getManualUpdateAction().setChecked(dimensionNamesAction->getPoints()->getNumDimensions() > MANUAL_UPDATE_THRESHOLD);

        if (dimensionNamesAction->getPoints()->getNumDimensions() < MANUAL_UPDATE_THRESHOLD)
            _timer.start(LAZY_UPDATE_INTERVAL);
    });

    connect(&dimensionNamesAction->getPoints(), &Dataset<Points>::dataSelectionChanged, this, [this, dimensionNamesAction, updateActions]() -> void {
        if (dimensionNamesAction->getManualUpdateAction().isChecked()) {
            _dirty = true;

            updateActions();
            return;
        }

        _timer.start(LAZY_UPDATE_INTERVAL);

        _dirty = false;

        updateActions();
    });

    updateActions();
    updateListView();
}
