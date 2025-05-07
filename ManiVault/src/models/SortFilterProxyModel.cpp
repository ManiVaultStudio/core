// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SortFilterProxyModel.h"

#include <QDebug>

#ifdef _DEBUG
    //#define SORT_FILTER_PROXY_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv
{

SortFilterProxyModel::SortFilterProxyModel(QObject* parent /*= nullptr*/, const QString& title /*= "SortFilterProxyModel"*/) :
    QSortFilterProxyModel(parent),
    Serializable(title),
    _rowTypeName("Item"),
    _numberOfRowsAction(this, "Number of rows"),
    _textFilterAction(this, "Text filter"),
    _textFilterSettingsAction(this, "Text filter settings"),
    _textFilterColumnAction(this, "Column"),
    _textFilterCaseSensitiveAction(this, "Case sensitive"),
    _textFilterRegularExpressionAction(this, "Regular expression", false)
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    _numberOfRowsAction.initialize(this);

    _textFilterAction.setConnectionPermissionsToForceNone();
    _textFilterAction.setPlaceHolderString("Search by title");
    _textFilterAction.setClearable(true);
    _textFilterAction.setSearchMode(true);

    _textFilterAction.setConnectionPermissionsToForceNone();
    _textFilterAction.setText("Filtering");
    _textFilterAction.setIconByName("filter");
    _textFilterAction.setToolTip("Adjust filtering parameters");

    _textFilterCaseSensitiveAction.setConnectionPermissionsToForceNone();
    _textFilterCaseSensitiveAction.setToolTip("Enable/disable search filter case-sensitive");

    _textFilterRegularExpressionAction.setConnectionPermissionsToForceNone();
    _textFilterRegularExpressionAction.setToolTip("Enable/disable search filter with regular expression");

    _textFilterSettingsAction.setConnectionPermissionsToForceNone();
    _textFilterSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _textFilterSettingsAction.setIconByName("filter");
    _textFilterSettingsAction.addAction(&_textFilterColumnAction);
    _textFilterSettingsAction.addAction(&_textFilterCaseSensitiveAction);
    _textFilterSettingsAction.addAction(&_textFilterRegularExpressionAction);

    _textFilterColumnAction.setConnectionPermissionsToForceNone();
    _textFilterColumnAction.setToolTip("Choose which column to use for text filtering");

    _textFilterCaseSensitiveAction.setConnectionPermissionsToForceNone();
    _textFilterCaseSensitiveAction.setToolTip("Enable/disable search filter case-sensitive");

    _textFilterRegularExpressionAction.setConnectionPermissionsToForceNone();
    _textFilterRegularExpressionAction.setToolTip("Enable/disable search filter with regular expression");

    connect(&_textFilterAction, &StringAction::stringChanged, this, &SortFilterProxyModel::updateTextFilterSettings);
    connect(&_textFilterCaseSensitiveAction, &ToggleAction::toggled, this, &SortFilterProxyModel::updateTextFilterSettings);
    connect(&_textFilterRegularExpressionAction, &ToggleAction::toggled, this, &SortFilterProxyModel::updateTextFilterSettings);

    connect(&_textFilterColumnAction, &OptionAction::currentIndexChanged, this, [this](const std::atomic_int32_t& currentIndex) -> void {
        setFilterColumn(currentIndex);
        updateTextFilterSettings();
    });
}

void SortFilterProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this, &QAbstractItemModel::columnsInserted, this, nullptr);
        disconnect(this, &QAbstractItemModel::columnsMoved, this, nullptr);
        disconnect(this, &QAbstractItemModel::columnsRemoved, this, nullptr);
        disconnect(this, &QAbstractItemModel::headerDataChanged, this, nullptr);
    }

    QSortFilterProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::columnsInserted, this, &SortFilterProxyModel::updateFilterColumnAction);
    connect(sourceModel, &QAbstractItemModel::columnsMoved, this, &SortFilterProxyModel::updateFilterColumnAction);
    connect(sourceModel, &QAbstractItemModel::columnsRemoved, this, &SortFilterProxyModel::updateFilterColumnAction);
    connect(sourceModel, &QAbstractItemModel::headerDataChanged, this, &SortFilterProxyModel::updateFilterColumnAction);

    updateFilterColumnAction();
    updateTextFilterSettings();
    setFilterColumn(0);
}

void SortFilterProxyModel::setRowTypeName(const QString& rowTypeName)
{
    if (rowTypeName == _rowTypeName)
        return;

    _rowTypeName = rowTypeName;

    updateTextFilterSettings();

    emit rowTypeNameChanged(_rowTypeName);
}

QString SortFilterProxyModel::getRowTypeName() const
{
    return _rowTypeName;
}

void SortFilterProxyModel::setFilterColumn(int column)
{
    QSortFilterProxyModel::setFilterKeyColumn(column);

    updateTextFilterSettings();
}

void SortFilterProxyModel::updateTextFilterSettings()
{
    const auto rowTypeNameLowered = _rowTypeName.toLower();

    if (_textFilterRegularExpressionAction.isChecked()) {
        const auto description = QString("Search for %1 by regular expression").arg(rowTypeNameLowered);

        _textFilterAction.setPlaceHolderString(description);
        _textFilterAction.setToolTip(description);

        auto regularExpression = QRegularExpression(_textFilterAction.getString());

        if (!_textFilterCaseSensitiveAction.isChecked())
            regularExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

        if (regularExpression.isValid() && (regularExpression != filterRegularExpression()))
            setFilterRegularExpression(regularExpression);
    }
    else {
        const auto filterColumn = headerData(filterKeyColumn(), Qt::Horizontal).toString().toLower();
        const auto description  = QString("Search for %1 by %2").arg(rowTypeNameLowered, filterColumn);

        _textFilterAction.setPlaceHolderString(description);
        _textFilterAction.setToolTip(description);

        if (QRegularExpression(_textFilterAction.getString()) != filterRegularExpression())
            setFilterFixedString(_textFilterAction.getString());
    }

    setFilterCaseSensitivity(_textFilterCaseSensitiveAction.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
    invalidate();
}

void SortFilterProxyModel::updateFilterColumnAction()
{
    const auto previousFilterColumn = _textFilterColumnAction.getCurrentText();

    QStringList columnNames;

    for (std::int32_t columnIndex = 0; columnIndex < columnCount(); columnIndex++)
        columnNames << headerData(columnIndex, Qt::Horizontal, Qt::EditRole).toString();

    _textFilterColumnAction.setOptions(columnNames);
    _textFilterColumnAction.setCurrentText(previousFilterColumn.isEmpty() ? "" : (columnNames.contains(previousFilterColumn) ? previousFilterColumn : ""));
}

void SortFilterProxyModel::fromVariantMap(const QVariantMap& variantMap)
{
	Serializable::fromVariantMap(variantMap);

    _textFilterAction.fromParentVariantMap(variantMap);
    _textFilterColumnAction.fromParentVariantMap(variantMap);
    _textFilterCaseSensitiveAction.fromParentVariantMap(variantMap);
    _textFilterRegularExpressionAction.fromParentVariantMap(variantMap);
}

QVariantMap SortFilterProxyModel::toVariantMap() const
{
	auto variantMap = Serializable::toVariantMap();

    _textFilterAction.insertIntoVariantMap(variantMap);
    _textFilterColumnAction.insertIntoVariantMap(variantMap);
    _textFilterCaseSensitiveAction.insertIntoVariantMap(variantMap);
    _textFilterRegularExpressionAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
