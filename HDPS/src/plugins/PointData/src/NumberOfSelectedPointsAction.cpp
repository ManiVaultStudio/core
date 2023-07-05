// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NumberOfSelectedPointsAction.h"

#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>

NumberOfSelectedPointsAction::NumberOfSelectedPointsAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent, "Number Of Selected Points"),
    _points(points),
    _numberOfSelectedPointsAction(this, "Number of selected points")
{
    setEnabled(false);
    setText("Number of selected points");
    setToolTip("Number of selected points");

    _numberOfSelectedPointsAction.setEnabled(false);
}

Dataset<Points>& NumberOfSelectedPointsAction::getPoints()
{
    return _points;
}

NumberOfSelectedPointsAction::Widget::Widget(QWidget* parent, NumberOfSelectedPointsAction* numberOfSelectedPointsAction) :
    WidgetActionWidget(parent, numberOfSelectedPointsAction),
    _timer()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(numberOfSelectedPointsAction->getNumberOfSelectedPointsAction().createWidget(this));

    setLayout(layout);
    
    _timer.setSingleShot(true);

    const auto updateNumberOfSelectedPoints = [this, numberOfSelectedPointsAction]() -> void {
        auto points = numberOfSelectedPointsAction->getPoints();

        if (!points.isValid())
            return;
        
        auto selection = points->getSelection<Points>();

        std::vector<std::uint32_t> selectedIndices;

        if (points->isFull()) {
            selectedIndices = selection->indices;
        }
        else {
            selectedIndices.clear();
            selectedIndices.reserve(points->indices.size());

            QSet<std::uint32_t> indicesSet(points->indices.begin(), points->indices.end());

            for (const auto& selectionIndex : selection->indices)
                if (indicesSet.contains(selectionIndex))
                    selectedIndices.push_back(selectionIndex);
        }

        numberOfSelectedPointsAction->getNumberOfSelectedPointsAction().setString(QString::number(selectedIndices.size()));
    };

    connect(&_timer, &QTimer::timeout, this, [this, updateNumberOfSelectedPoints]() -> void {
        if (_timer.isActive())
            _timer.start(LAZY_UPDATE_INTERVAL);
        else {
            _timer.stop();
            updateNumberOfSelectedPoints();
        }
    });

    connect(&numberOfSelectedPointsAction->getPoints(), &Dataset<Points>::dataSelectionChanged, this, [this]() -> void {
        _timer.start(LAZY_UPDATE_INTERVAL);
    });

    updateNumberOfSelectedPoints();
}
