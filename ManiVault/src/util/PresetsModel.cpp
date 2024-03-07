// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PresetsModel.h"

namespace mv {

namespace util {

PresetsModel::PresetsModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _presets()
{
}

int PresetsModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int PresetsModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    return _presets.count();
}

QVariant PresetsModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    const auto preset = _presets.at(index.row());
    const auto column = static_cast<Column>(index.column());

    switch (role) {
        case Qt::DecorationRole:
        {
            switch (column) {
                case Column::Name:
                    return preset.getIcon();
            }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (column) {
                case Column::Name:
                    return data(index, Qt::EditRole).toString() + (preset.isDivergent() ? "*" : "");

                case Column::Description:
                    return data(index, Qt::EditRole).toString();
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (column) {
                case Column::Name:
                    return preset.getName();

                case Column::Description:
                    return preset.getDescription();
            }

            break;
        }

        case Qt::TextAlignmentRole:
            return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);

        default:
            break;
    }

    return QVariant();
}

void PresetsModel::setWidgetAction(mv::gui::WidgetAction* widgetAction)
{
}

}
}
