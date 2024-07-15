// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconPickerAction.h"

namespace mv::gui {

IconPickerAction::IconPickerAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _filePickerAction(this, "FilePicker"),
    _iconAction(this, "Icon")
{
    addAction(&_filePickerAction);
    addAction(&_iconAction);

    connect(&_iconAction, &IconAction::iconChanged, this, &IconPickerAction::iconChanged);
}

const QIcon IconPickerAction::getIcon() const
{
    return _iconAction.getIcon();
}

void IconPickerAction::setIcon(const QIcon& icon)
{
    _iconAction.setIcon(icon);
}

void IconPickerAction::setIconFromImage(const QImage& image)
{
    setIcon(createIcon(QPixmap::fromImage(image)));
}

void IconPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _filePickerAction.fromParentVariantMap(variantMap);
    _iconAction.fromParentVariantMap(variantMap);
}

QVariantMap IconPickerAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _filePickerAction.insertIntoVariantMap(variantMap);
    _iconAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
