// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NumberOfRowsAction.h"

namespace mv::gui {

NumberOfRowsAction::NumberOfRowsAction(QObject* parent, const QString& title) :
    StringAction(parent, title),
    _model(nullptr),
    _updateTimer()
{
    setToolTip("Number of items");
    setEnabled(false);

    _updateTimer.setSingleShot(true);
    _updateTimer.setInterval(100);

    connect(&_updateTimer, &QTimer::timeout, this, &NumberOfRowsAction::updateString);
}

void NumberOfRowsAction::initialize(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (!model)
        return;

    if (model == _model)
        return;

    if (_model) {
        disconnect(_model, &QAbstractItemModel::rowsInserted, this, nullptr);
        disconnect(_model, &QAbstractItemModel::rowsRemoved, this, nullptr);
        disconnect(_model, &QAbstractItemModel::layoutChanged, this, nullptr);
    }

    _model = model;

    connect(_model, &QAbstractItemModel::rowsInserted, this, &NumberOfRowsAction::numberOfRowsChanged);
    connect(_model, &QAbstractItemModel::rowsRemoved, this, &NumberOfRowsAction::numberOfRowsChanged);
    connect(_model, &QAbstractItemModel::layoutChanged, this, &NumberOfRowsAction::numberOfRowsChanged);

    updateString();
}

void NumberOfRowsAction::updateString()
{
    Q_ASSERT(_model);

    if (!_model)
        return;

    setString(QString::number(_model->rowCount()));

    _updateTimer.start();
}

void NumberOfRowsAction::numberOfRowsChanged()
{
    if (_updateTimer.isActive())
        return;

    updateString();
}

}
