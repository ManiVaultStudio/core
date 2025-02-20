// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconPickerAction.h"

#include "util/Icon.h"

#include <QFileInfo>

namespace mv::gui {

IconPickerAction::IconPickerAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _inputFilePathPickerAction(this, "FilePicker"),
    _iconAction(this, "Icon")
{
    setShowLabels(false);

    addAction(&_inputFilePathPickerAction);
    addAction(&_iconAction);

    connect(&_inputFilePathPickerAction, &FilePickerAction::filePathChanged, this, [this](const QString& filePath) -> void
    {
        if (!QFileInfo(filePath).exists())
            return;

        QImage png(filePath);

        if (png.isNull())
            return;

        _iconAction.setIconFromImage(png);
    });

    connect(&_iconAction, &QAction::changed, this, [this]() -> void {
        emit iconChanged(_iconAction.icon());
    });

    _inputFilePathPickerAction.setStretch(1);
    _inputFilePathPickerAction.setNameFilters({ "*.png" });
    _inputFilePathPickerAction.setDefaultWidgetFlags(FilePickerAction::WidgetFlag::PushButton);
}

const util::StyledIcon& IconPickerAction::getIcon() const
{
    return _iconAction.getIcon();
}

void IconPickerAction::setIcon(const util::StyledIcon& icon)
{
    _iconAction.setIcon(icon);
}

void IconPickerAction::setIconFromImage(const QImage& image)
{
    setIcon(util::StyledIcon(createIcon(QPixmap::fromImage(image))));
}

void IconPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _inputFilePathPickerAction.fromParentVariantMap(variantMap);
    _iconAction.fromParentVariantMap(variantMap);
}

QVariantMap IconPickerAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _inputFilePathPickerAction.insertIntoVariantMap(variantMap);
    _iconAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
