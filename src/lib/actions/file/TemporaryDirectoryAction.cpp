// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TemporaryDirectoryAction.h"

#include "util/Serialization.h"

#include <QDir>

using namespace mv::util;

namespace mv::gui
{

TemporaryDirectoryAction::TemporaryDirectoryAction(QObject* parent, const QString& title, const QString& directory /*= QString()*/, const QString& defaultDirectory /*= QString()*/) :
    HorizontalGroupAction(parent, title),
    _directoryPickerAction(this, "Directory", directory),
    _resetAction(this, "Reset"),
    _defaultDirectory(QDir::cleanPath(defaultDirectory.isEmpty() ? QDir::tempPath() : defaultDirectory))
{
    setShowLabels(false);

    _directoryPickerAction.setStretch(1);
    _resetAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _resetAction.setIconByName("rotate-left");
    _resetAction.setToolTip("Reset to the default temporary directory");

    addAction(&_directoryPickerAction);
    addAction(&_resetAction);

    connect(&_directoryPickerAction, &DirectoryPickerAction::directoryChanged, this, [this](const QString& directory) {
        saveToSettings();
        emit directoryChanged(directory);
    });

    connect(&_resetAction, &TriggerAction::triggered, this, &TemporaryDirectoryAction::reset);
}

QString TemporaryDirectoryAction::getDirectory() const
{
    return _directoryPickerAction.getDirectory();
}

void TemporaryDirectoryAction::setDirectory(const QString& directory)
{
    _directoryPickerAction.setDirectory(directory);
}

QString TemporaryDirectoryAction::getDefaultDirectory() const
{
    return _defaultDirectory;
}

void TemporaryDirectoryAction::reset()
{
    setDirectory(_defaultDirectory);
}

void TemporaryDirectoryAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    if (variantMap.contains("Value"))
        setDirectory(variantMap["Value"].toString());
}

QVariantMap TemporaryDirectoryAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    variantMap.insert("Value", getDirectory());

    return variantMap;
}

}
