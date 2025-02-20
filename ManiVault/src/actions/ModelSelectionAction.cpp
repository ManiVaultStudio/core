// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModelSelectionAction.h"

#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace mv::gui {

ModelSelectionAction::ModelSelectionAction(QObject* parent, const QString& title, QItemSelectionModel* selectionModel /*= nullptr*/) :
    HorizontalGroupAction(parent, title),
    _selectionModel(nullptr),
    _sourceModel(nullptr),
    _selectAllAction(this, "All"),
    _clearSelectionAction(this, "Clear"),
    _invertSelectionAction(this, "Invert")
{
    setText(title);
    setToolTip("Item selection");
    setIconByName("mouse-pointer");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setDefaultWidgetFlags(WidgetFlag::Default);

    addAction(&_selectAllAction);
    addAction(&_clearSelectionAction);
    addAction(&_invertSelectionAction);

    if (selectionModel)
        initialize(selectionModel);
}

void ModelSelectionAction::initialize(QItemSelectionModel* selectionModel)
{
    Q_ASSERT(selectionModel);

    if (!selectionModel)
        return;

    _selectionModel = selectionModel;
    _sourceModel    = _selectionModel->model();

    const auto updateReadOnly = [this]() -> void {
        const auto numberOfSelectedRows = _selectionModel->selectedRows().count();

        _selectAllAction.setEnabled(numberOfSelectedRows < _sourceModel->rowCount());
        _clearSelectionAction.setEnabled(numberOfSelectedRows >= 1);
    };

    connect(_selectionModel, &QItemSelectionModel::selectionChanged, this, updateReadOnly);

    connect(&_selectAllAction, &TriggerAction::triggered, this, [this]() -> void {
        QItemSelection itemSelection(_sourceModel->index(0, 0), _sourceModel->index(_sourceModel->rowCount() - 1, _sourceModel->columnCount() - 1));

        _selectionModel->select(itemSelection, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
    });

    connect(&_clearSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        QItemSelection itemSelection(_sourceModel->index(0, 0), _sourceModel->index(_sourceModel->rowCount() - 1, _sourceModel->columnCount() - 1));

        _selectionModel->select(itemSelection, QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
    });

    connect(&_invertSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        QItemSelection itemSelection(_sourceModel->index(0, 0), _sourceModel->index(_sourceModel->rowCount() - 1, _sourceModel->columnCount() - 1));

        _selectionModel->select(itemSelection, QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
    });
}

}
