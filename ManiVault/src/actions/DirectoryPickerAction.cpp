// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DirectoryPickerAction.h"
#include "Application.h"

#include "widgets/FileDialog.h"

#include <QDir>
#include <QHBoxLayout>

using namespace mv::util;

namespace mv::gui {

DirectoryPickerAction::DirectoryPickerAction(QObject* parent, const QString& title, const QString& directory /*= QString()*/) :
    WidgetAction(parent, title),
    _directoryAction(this, "Directory"),
    _pickAction(this, "Pick"),
    _useNativeDialog(true)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setDirectory(directory);

    _completer.setModel(&_dirModel);

    // Show directory line edit action
    _directoryAction.getTrailingAction().setVisible(true);
    _directoryAction.setCompleter(&_completer);

    // Configure pick action
    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIconByName("folder");
    _pickAction.setToolTip("Click to choose a directory");

    // Disable trailing action
    _directoryAction.getTrailingAction().setEnabled(false);

    // Update the trailing action of the string action to indicate whether the (typed) directory is valid or not
    const auto updateStatusAction = [this]() -> void {
        _directoryAction.getTrailingAction().setIcon(isValid() ? StyledIcon("check") : StyledIcon("exclamation"));
        _directoryAction.getTrailingAction().setToolTip(isValid() ? "Directory exists and is valid" : "Directory does not exist");
    };

    // Initial update of the status action
    updateStatusAction();

    // Update the line edit string when the directory string changes
    connect(&_directoryAction, &StringAction::stringChanged, this, updateStatusAction);

    // Open file dialog when pick action is triggered
    connect(&_pickAction, &TriggerAction::triggered, this, [this]() {
        QFileDialog::Options options = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

        if (!_useNativeDialog)
            options.setFlag(QFileDialog::DontUseNativeDialog);

        const auto directory = FileDialog::getExistingDirectory(nullptr, tr("Open directory"), "", options);

        if (!directory.isEmpty())
            setDirectory(directory);
    });

    // Pass-through action signals
    connect(&_directoryAction, &StringAction::stringChanged, this, &DirectoryPickerAction::directoryChanged);
    connect(&_directoryAction, &StringAction::placeholderStringChanged, this, &DirectoryPickerAction::placeholderStringChanged);
}

QString DirectoryPickerAction::getDirectory() const
{
    return _directoryAction.getString();
}

void DirectoryPickerAction::setDirectory(const QString& directory)
{
    if (directory == getDirectory())
        return;

    _directoryAction.setString(directory);

    saveToSettings();
}

QString DirectoryPickerAction::getPlaceholderString() const
{
    return _directoryAction.getPlaceholderString();
}

void DirectoryPickerAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == getPlaceholderString())
        return;

    _directoryAction.setPlaceHolderString(placeholderString);
}

QString DirectoryPickerAction::getDirectoryName() const
{
    return QDir(getDirectory()).dirName();
}

void DirectoryPickerAction::setUseNativeFileDialog(bool useNativeDialog)
{
    _useNativeDialog = useNativeDialog;
}

bool DirectoryPickerAction::getUseNativeFileDialog() const
{
    return _useNativeDialog;
}

bool DirectoryPickerAction::isValid() const
{
    return !getDirectory().isEmpty() && QDir(getDirectory()).exists();
}

void DirectoryPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    if (variantMap.contains("Value"))
        setDirectory(variantMap["Value"].toString());
}

QVariantMap DirectoryPickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getDirectory() }
    });

    return variantMap;
}

QWidget* DirectoryPickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(_directoryAction.createWidget(parent));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(_pickAction.createWidget(parent));

    widget->setLayout(layout);

    return widget;
}

}
