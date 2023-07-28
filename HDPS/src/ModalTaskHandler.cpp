// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "ModalTask.h"

#include "CoreInterface.h"

namespace hdps {

using namespace gui;

ModalTaskHandler::ModalTasksDialog ModalTaskHandler::modalTasksDialog = ModalTasksDialog(nullptr);

ModalTaskHandler::ModalTaskHandler(QObject* parent /*= nullptr*/) :
    AbstractTaskHandler(parent)
{
}

void ModalTaskHandler::init()
{
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _tasksAction(this, "Modal Tasks")
{
    //setModal(true);

    auto layout = new QVBoxLayout();

    layout->addWidget(_tasksAction.createWidget(this));

    setLayout(layout);

    auto& taskFilterModel = _tasksAction.getTasksFilterModel();

    const auto updateVisibility = [this]() -> void {
        const auto numberOfRows = _tasksAction.getTasksModel().rowCount();

        for (int i = 0; i < numberOfRows; i++)
            qDebug() << _tasksAction.getTasksModel().index(i, 0).data(Qt::DisplayRole).toString();

        if (numberOfRows == 0 && isVisible())
            close();

        if (numberOfRows >= 1 && !isVisible())
            open();
    };

    updateVisibility();

    connect(&taskFilterModel, &QSortFilterProxyModel::rowsInserted, updateVisibility);
    connect(&taskFilterModel, &QSortFilterProxyModel::rowsRemoved, updateVisibility);
    connect(&taskFilterModel, &QSortFilterProxyModel::layoutChanged, updateVisibility);
}

}
