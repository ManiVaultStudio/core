// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModelColumnsAction.h"

#include <QAbstractItemModel>
#include <QHeaderView>

namespace mv::gui {

ModelColumnsAction::ModelColumnsAction(QObject* parent, const QString& title, QAbstractItemModel* model /*= nullptr*/, QHeaderView* horizontalHeaderView /*= nullptr*/) :
    VerticalGroupAction(parent, title),
    _model(nullptr),
    _horizontalHeaderView(nullptr),
    _columnsAction(this, "Columns")
{
    setText(title);
    setToolTip("Columns toggle");
    setIconByName("table");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setDefaultWidgetFlags(WidgetFlag::Default);

    addAction(&_columnsAction);

    if (model && horizontalHeaderView)
        initialize(model, horizontalHeaderView);
}

void ModelColumnsAction::initialize(QAbstractItemModel* model, QHeaderView* horizontalHeaderView)
{
    Q_ASSERT(model && horizontalHeaderView);

    if (!model || !horizontalHeaderView)
        return;

    _model                  = model;
    _horizontalHeaderView   = horizontalHeaderView;

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    connect(_model, &QAbstractItemModel::columnsInserted, this, &ModelColumnsAction::updateColumnsOptionsAction);
    connect(_model, &QAbstractItemModel::columnsRemoved, this, &ModelColumnsAction::updateColumnsOptionsAction);

    updateColumnsOptionsAction();
}

void ModelColumnsAction::updateColumnsOptionsAction()
{
    QStringList columnsOptions, selectedColumnsOptions;

    for (int columnIndex = 0; columnIndex < _model->columnCount(); ++columnIndex) {
        const auto columnName = _model->headerData(columnIndex, Qt::Horizontal).toString();

        columnsOptions << columnName;

        if (!_horizontalHeaderView->isSectionHidden(columnIndex))
            selectedColumnsOptions << columnName;
    }

    _columnsAction.initialize(columnsOptions);
}

}
