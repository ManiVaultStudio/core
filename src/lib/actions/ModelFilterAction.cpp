// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModelFilterAction.h"

#include <QSortFilterProxyModel>

namespace mv::gui {

ModelFilterAction::ModelFilterAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr),
    _filterNameAction(this, "Name"),
    _filterColumnAction(this, "Column"),
    _filterGroupAction(this, "Filter"),
    _filterCaseSensitiveAction(this, "Case-sensitive", false),
    _filterRegularExpressionAction(this, "Regular expression", false)
{
    setShowLabels(false);
    setConnectionPermissionsToForceNone(true);

    _filterNameAction.setSearchMode(true);
    _filterNameAction.setClearable(true);
    _filterNameAction.setConnectionPermissionsToForceNone();

    _filterGroupAction.setText("Filtering");
    _filterGroupAction.setIconByName("filter");
    _filterGroupAction.setToolTip("Adjust filtering parameters");
    _filterGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _filterCaseSensitiveAction.setToolTip("Enable/disable search filter case-sensitive");

    _filterRegularExpressionAction.setToolTip("Enable/disable search filter with regular expression");

    _filterGroupAction.addAction(&_filterColumnAction);
    _filterGroupAction.addAction(&_filterCaseSensitiveAction);
    _filterGroupAction.addAction(&_filterRegularExpressionAction);

    addAction(&_filterNameAction);
    addAction(&_filterGroupAction);

    connect(&_filterNameAction, &StringAction::stringChanged, this, &ModelFilterAction::updateFilterModel);
    connect(&_filterColumnAction, &OptionAction::currentIndexChanged, this, &ModelFilterAction::updateFilterModel);
    connect(&_filterCaseSensitiveAction, &ToggleAction::toggled, this, &ModelFilterAction::updateFilterModel);
    connect(&_filterRegularExpressionAction, &ToggleAction::toggled, this, &ModelFilterAction::updateFilterModel);
}

void ModelFilterAction::initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName)
{
    Q_ASSERT(model && filterModel);

    if (!model || !filterModel)
        return;

    _model          = model;
    _filterModel    = filterModel;
    _itemTypeName   = itemTypeName;

    updateFilterModel();
}

void ModelFilterAction::updateFilterModel()
{
    if (!_filterModel)
        return;

    const auto itemTypeNameLowered = _itemTypeName.toLower();

    _filterModel->setFilterKeyColumn(_filterColumnAction.getCurrentIndex());

    auto shouldInvalidateFilterModel = false;

    if (_filterRegularExpressionAction.isChecked()) {
        _filterNameAction.setPlaceHolderString(QString("Search for %1 by regular expression").arg(itemTypeNameLowered));

        auto regularExpression = QRegularExpression(_filterNameAction.getString());

        if (!_filterCaseSensitiveAction.isChecked())
            regularExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

        if (regularExpression.isValid() && (regularExpression != _filterModel->filterRegularExpression())) {
            _filterModel->setFilterRegularExpression(regularExpression);

            shouldInvalidateFilterModel = true;
        }
    }
    else {
        const auto filterColumn = _model->headerData(_filterModel->filterKeyColumn(), Qt::Horizontal).toString().toLower();

        _filterNameAction.setPlaceHolderString(QString("Search for %1 in %2").arg(itemTypeNameLowered, filterColumn));

        if (QRegularExpression(_filterNameAction.getString()) != _filterModel->filterRegularExpression()) {
            _filterModel->setFilterFixedString(_filterNameAction.getString());

            shouldInvalidateFilterModel = true;
        }
    }

    _filterModel->setFilterCaseSensitivity(_filterCaseSensitiveAction.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (shouldInvalidateFilterModel)
        _filterModel->invalidate();
}

}
