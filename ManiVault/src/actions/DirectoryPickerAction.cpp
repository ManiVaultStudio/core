// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DirectoryPickerAction.h"
#include "WidgetActionExampleWidget.h"

#include <Application.h>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>

using namespace mv::util;

namespace mv::gui {

DirectoryPickerAction::DirectoryPickerAction(QObject* parent, const QString& title, const QString& directory /*= QString()*/) :
    WidgetAction(parent, title),
    _dirModel(),
    _completer(),
    _directoryAction(this, "Directory"),
    _pickAction(this, "Pick")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setDirectory(directory);

    _completer.setModel(&_dirModel);

    _directoryAction.setStretch(1);

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
        _directoryAction.getTrailingAction().setIcon(isValid() ? Application::getIconFont("FontAwesome").getIcon("check") : Application::getIconFont("FontAwesome").getIcon("exclamation"));
        _directoryAction.getTrailingAction().setToolTip(isValid() ? "Directory exists and is valid" : "Directory does not exist");
    };

    // Initial update of the status action
    updateStatusAction();

    // Update the line edit string when the directory string changes
    connect(&_directoryAction, &StringAction::stringChanged, this, updateStatusAction);

    // Open file dialog when pick action is triggered
    connect(&_pickAction, &TriggerAction::triggered, this, [this]() {
        const auto directory = QFileDialog::getExistingDirectory(nullptr, tr("Open directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

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
        layout->addWidget(_directoryAction.createWidget(parent), _directoryAction.getStretch());

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(_pickAction.createWidget(parent), _pickAction.getStretch());

    widget->setLayout(layout);

    return widget;
}

QWidget* DirectoryPickerAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget          = new WidgetActionExampleWidget(parent);
    auto directoryPickerAction  = new DirectoryPickerAction(exampleWidget, "Example");

    directoryPickerAction->setPlaceHolderString("This placeholder text can be changed with mv::gui::DirectoryPickerAction::setPlaceHolderString(const QString& placeholderString)");

    const QString markdownText =
        "## General \n\n"
        "`mv::gui::DirectoryPickerAction` provides a GUI for picking a directory on the system. It gives visual feedback on whether a directory is valid or not (as a trailing icon). \n\n"
        "By default it consists of a `line edit` and a `picker push button` control, but these can be toggled individually by: \n\n"
        "- Setting default widget flags: \n\n"
        "  `directoryPickerAction->setDefaultWidgetFlags(DirectoryPickerAction::WidgetFlag::LineEdit)` \n\n"
        "  `directoryPickerAction->setDefaultWidgetFlag(DirectoryPickerAction::WidgetFlag::LineEdit, true/false)` \n\n"
        "- Setting the widget flags during widget creation (e.g. `directoryPickerAction->createWidget(parentWidget, DirectoryPickerAction::WidgetFlag::LineEdit)`) \n\n"
        "## Usage \n\n"
        "[#include <actions/DirectoryPickerAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/DirectoryPickerAction.h)\n\n"
        "To use: \n\n"
        "- Manually edit the path in the text edit \n\n"
        "- Click on the button next to the text edit to pick a directory \n\n"
        "## API \n\n"
        "The `mv::gui::DirectoryPickerAction` API inherits from `mv::gui::WidgetAction` \n\n"
        "### Methods \n\n"
        "- `QString getDirectory() const` \n\n"
        "  Get the current directory \n\n"
        "- `void setDirectory(const QString& directory)` \n\n"
        "  Sets the current directory to `directory` \n\n"
        "- `QString getPlaceholderString()` \n\n"
        "  Gets the current placeholder string (string that is shown when there is no user input) \n\n"
        "- `void setPlaceHolderString(const QString& placeholderString)` \n\n"
        "  Sets the placeholder text to `placeholderString` \n\n"
        "- `QString getDirectoryName()` \n\n"
        "  Get the name of the current directory \n\n"
        "- `bool isValid() const` \n\n"
        "  Determines whether the directory actually exists \n\n"
        "### Signals \n\n"
        "- `void directoryChanged(const QString& directory)` \n\n"
        "  Signals that the current directory changed to `directory` \n\n"
        "- `void placeholderStringChanged(const QString& placeholderString)` \n\n"
        "  Signals that the placeholder string changed to `placeholderString` \n\n";

    exampleWidget->addWidget(directoryPickerAction->createWidget(exampleWidget));
    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}
