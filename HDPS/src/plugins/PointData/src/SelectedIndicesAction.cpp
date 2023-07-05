#include "SelectedIndicesAction.h"

#include <QGridLayout>
#include <QListView>
#include <QSet>

using namespace hdps;
using namespace hdps::gui;

SelectedIndicesAction::SelectedIndicesAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent, "Selected Indices"),
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
    auto selectedIndicesListView = new ListView();

    //setMinimumWidth(0);
    selectedIndicesListView->setFixedHeight(100);
    //selectedIndicesListView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    //selectedIndicesListView->setFixedWidth(1);
    selectedIndicesListView->setProperty("sizeHint", QSize(0, 0));

    auto selectedIndicesLayout = new QVBoxLayout();

    selectedIndicesLayout->setContentsMargins(0, 0, 0, 0);

    selectedIndicesLayout->addWidget(selectedIndicesListView);
    selectedIndicesLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    selectedIndicesLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createWidget(this));

    setLayout(selectedIndicesLayout);

    _timer.setSingleShot(true);

    const auto updateListView = [this, selectedIndicesAction, selectedIndicesListView]() -> void {
        QStringList items;

        for (auto selectedIndex : selectedIndicesAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListView->setModel(new QStringListModel(items));
    };

    const auto updateActions = [this, selectedIndicesAction]() -> void {
        selectedIndicesAction->getUpdateAction().setEnabled(selectedIndicesAction->getManualUpdateAction().isChecked() && _dirty);
    };

    connect(&selectedIndicesAction->getUpdateAction(), &TriggerAction::triggered, this, [this, updateListView, updateActions]() -> void {
        updateListView();

        _dirty = false;

        updateActions();
    });

    connect(&selectedIndicesAction->getManualUpdateAction(), &TriggerAction::triggered, this, updateActions);

    connect(&_timer, &QTimer::timeout, this, [this, updateListView]() -> void {
        if (_timer.isActive())
            _timer.start(LAZY_UPDATE_INTERVAL);
        else {
            _timer.stop();
            updateListView();
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
    updateListView();
}
