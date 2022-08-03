#include "SelectedIndicesAction.h"

#include <event/Event.h>

#include <QGridLayout>
#include <QListView>

#include <QSet>

using namespace hdps;
using namespace hdps::gui;

SelectedIndicesAction::SelectedIndicesAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent),
    _points(points),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update")
{
    setText("Selected indices");
}

Dataset<Points>& SelectedIndicesAction::getPoints()
{
    return _points;
}

std::vector<std::uint32_t> SelectedIndicesAction::getSelectedIndices() const
{
    if (!_points.isValid())
        return std::vector<std::uint32_t>();

    auto selection = _points->getSelection<Points>();

    std::vector<std::uint32_t> selectedIndices;

    if (_points->isFull()) {
        selectedIndices = selection->indices;
    }
    else {
        selectedIndices.clear();
        selectedIndices.reserve(_points->indices.size());

        QSet<std::uint32_t> indicesSet(_points->indices.begin(), _points->indices.end());

        for (const auto& selectionIndex : selection->indices)
            if (indicesSet.contains(selectionIndex))
                selectedIndices.push_back(selectionIndex);
    }

    return selectedIndices;
}

SelectedIndicesAction::Widget::Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction) :
    WidgetActionWidget(parent, selectedIndicesAction),
    _timer(),
    _dirty(false)
{
    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(100);

    auto selectedIndicesLayout = new QHBoxLayout();

    selectedIndicesLayout->setContentsMargins(0, 0, 0, 0);
    selectedIndicesLayout->addWidget(selectedIndicesListWidget);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createWidget(this));
    updateLayout->addStretch(1);

    selectedIndicesLayout->addLayout(updateLayout);

    setLayout(selectedIndicesLayout);

    _timer.setSingleShot(true);

    const auto updateSelectedIndicesWidget = [this, selectedIndicesAction, selectedIndicesListWidget]() -> void {
        QStringList items;

        for (auto selectedIndex : selectedIndicesAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListWidget->setModel(new QStringListModel(items));
    };

    const auto updateActions = [this, selectedIndicesAction]() -> void {
        selectedIndicesAction->getUpdateAction().setEnabled(selectedIndicesAction->getManualUpdateAction().isChecked() && _dirty);
    };

    connect(&selectedIndicesAction->getUpdateAction(), &TriggerAction::triggered, this, [this, updateSelectedIndicesWidget, updateActions]() -> void {
        updateSelectedIndicesWidget();

        _dirty = false;

        updateActions();
    });

    connect(&selectedIndicesAction->getManualUpdateAction(), &TriggerAction::triggered, this, updateActions);

    connect(&_timer, &QTimer::timeout, this, [this, updateSelectedIndicesWidget]() -> void {
        if (_timer.isActive())
            _timer.start(LAZY_UPDATE_INTERVAL);
        else {
            _timer.stop();
            updateSelectedIndicesWidget();
        }
    });

    connect(&selectedIndicesAction->getPoints(), &Dataset<Points>::dataChanged, this, [this, selectedIndicesAction]() -> void {
        selectedIndicesAction->getManualUpdateAction().setChecked(selectedIndicesAction->getPoints()->getNumPoints() > MANUAL_UPDATE_THRESHOLD);

        if (selectedIndicesAction->getPoints()->getNumPoints() < MANUAL_UPDATE_THRESHOLD)
            _timer.start(LAZY_UPDATE_INTERVAL);
    });

    connect(&selectedIndicesAction->getPoints(), &Dataset<Points>::dataSelectionChanged, this, [this, selectedIndicesAction, updateActions]() -> void {
        if (selectedIndicesAction->getManualUpdateAction().isChecked()) {
            _dirty = true;

            updateActions();
            return;
        }

        _timer.start(LAZY_UPDATE_INTERVAL);

        _dirty = false;

        updateActions();
    });

    updateActions();
    updateSelectedIndicesWidget();
}
