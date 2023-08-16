// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

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
    _timer()
{
    auto selectedIndicesListView    = new ListView();
    auto selectedIndicesLayout      = new QVBoxLayout();

    selectedIndicesLayout->setContentsMargins(0, 0, 0, 0);

    selectedIndicesLayout->addWidget(selectedIndicesListView);
    selectedIndicesLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    selectedIndicesLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createWidget(this));

    selectedIndicesListView->setToolTip("Update this list manually if the data set contains more than " + QString::number(MANUAL_UPDATE_THRESHOLD) + " points");

    setLayout(selectedIndicesLayout);

    _timer.setSingleShot(true);

    const auto fewerPointsThanThreshold = [selectedIndicesAction]() -> bool {
        return selectedIndicesAction->getPoints()->getNumPoints() < MANUAL_UPDATE_THRESHOLD;
        };

    const auto updateListView = [this, selectedIndicesAction, selectedIndicesListView]() -> void {
        QStringList items;
        items.reserve(selectedIndicesAction->getSelectedIndices().size());

        for (auto selectedIndex : selectedIndicesAction->getSelectedIndices())
            items.emplace_back(QString::number(selectedIndex));

        selectedIndicesListView->setModel(new QStringListModel(items, selectedIndicesListView));
    };

    const auto enableUpdateTrigger = [this, selectedIndicesAction](bool enable) -> void {
        selectedIndicesAction->getUpdateAction().setEnabled(enable);
    };

    connect(&selectedIndicesAction->getUpdateAction(), &TriggerAction::triggered, this, [this, updateListView, enableUpdateTrigger]() -> void {
        updateListView();
        enableUpdateTrigger(false);
    });

    connect(&selectedIndicesAction->getManualUpdateAction(), &ToggleAction::toggled, this, enableUpdateTrigger);

    connect(&_timer, &QTimer::timeout, this, [this, updateListView, fewerPointsThanThreshold, selectedIndicesAction]() -> void {
        if (!fewerPointsThanThreshold())
            return;

        if (_timer.isActive())
            _timer.start(LAZY_UPDATE_INTERVAL);
        else {
            _timer.stop();
            updateListView();
        }
    });

    connect(&selectedIndicesAction->getPoints(), &Dataset<Points>::dataChanged, this, [this, selectedIndicesAction, fewerPointsThanThreshold]() -> void {
        selectedIndicesAction->getManualUpdateAction().setChecked(fewerPointsThanThreshold());

        if (fewerPointsThanThreshold())
            _timer.start(LAZY_UPDATE_INTERVAL);
    });

    connect(&selectedIndicesAction->getPoints(), &Dataset<Points>::dataSelectionChanged, this, [this, selectedIndicesAction, enableUpdateTrigger]() -> void {
        if (selectedIndicesAction->getManualUpdateAction().isChecked()) {
            enableUpdateTrigger(true);
            return;
        }

        _timer.start(LAZY_UPDATE_INTERVAL);
        enableUpdateTrigger(false);
    });

    enableUpdateTrigger(selectedIndicesAction->getManualUpdateAction().isChecked());

    if (fewerPointsThanThreshold())
        updateListView();
}
