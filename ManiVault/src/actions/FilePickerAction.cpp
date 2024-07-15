// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FilePickerAction.h"

#include <Application.h>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QStandardPaths>

using namespace mv::util;

namespace mv::gui {

FilePickerAction::FilePickerAction(QObject* parent, const QString& title, const QString& filePath /*= QString()*/, bool populateCompleter /*= true*/) :
    WidgetAction(parent, title),
    _dirModel(nullptr),
    _completer(nullptr),
    _filePathAction(this, "File path"),
    _pickAction(this, "Pick"),
    _nameFilters(),
    _defaultSuffix(),
    _fileType("File")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setFilePath(filePath);

    _filePathAction.setStretch(1);
    _filePathAction.getTrailingAction().setVisible(true);

    if (populateCompleter)
    {
        _dirModel = new QFileSystemModel(this);
        _completer = new QCompleter(this);
        _completer->setModel(_dirModel.get());
        _filePathAction.setCompleter(_completer.get());
    }

    _pickAction.setStretch(0);
    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIconByName("folder-open");
    
    const auto updatePickActionToolTip = [this]() -> void {
        _pickAction.setToolTip(QString("Click to browse for %1").arg(getFileType().toLower()));
    };

    updatePickActionToolTip();

    connect(this, &FilePickerAction::fileTypeChanged, this, updatePickActionToolTip);

    _filePathAction.getTrailingAction().setEnabled(false);

    const auto updateStatusAction = [this]() -> void {
        _filePathAction.getTrailingAction().setIcon(isValid() ? Application::getIconFont("FontAwesome").getIcon("check") : Application::getIconFont("FontAwesome").getIcon("exclamation"));
        _filePathAction.getTrailingAction().setToolTip(isValid() ? "File exists" : "File does not exist");
    };

    updateStatusAction();

    connect(&_filePathAction, &StringAction::stringChanged, this, updateStatusAction);

    connect(&_pickAction, &TriggerAction::triggered, this, [this]() {
        auto* fileDialog = new QFileDialog(nullptr, Qt::WindowStaysOnTopHint);

        fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog->setWindowTitle(QString("Open %1").arg(getFileType()));
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->setFileMode(QFileDialog::ExistingFile);
        fileDialog->setNameFilters(getNameFilters());
        fileDialog->setDefaultSuffix(getDefaultSuffix());
        fileDialog->setDirectory(Application::current()->getSetting(getSettingsPrefix(), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
        fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);

		connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            const auto filePath = fileDialog->selectedFiles().first();

            setFilePath(filePath);

            if (!getSettingsPrefix().isEmpty())
                setFilePath(QFileInfo(filePath).absoluteFilePath());
		});
        connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

        fileDialog->open();

    });

    connect(&_filePathAction, &StringAction::stringChanged, this, &FilePickerAction::filePathChanged);
    connect(&_filePathAction, &StringAction::placeholderStringChanged, this, &FilePickerAction::placeholderStringChanged);

    setFilePath(filePath);
}

QString FilePickerAction::getFilePath() const
{
    return _filePathAction.getString();
}

void FilePickerAction::setFilePath(const QString& filePath)
{
    if (filePath == getFilePath())
        return;

    _filePathAction.setString(filePath);

    saveToSettings();
}

QStringList FilePickerAction::getNameFilters() const
{
    return _nameFilters;
}

void FilePickerAction::setNameFilters(const QStringList& nameFilters)
{
    _nameFilters = nameFilters;
}

QString FilePickerAction::getDefaultSuffix() const
{
    return _defaultSuffix;
}

void FilePickerAction::setDefaultSuffix(const QString& defaultSuffix)
{
    _defaultSuffix = defaultSuffix;
}

QString FilePickerAction::getFileType() const
{
    return _fileType;
}

void FilePickerAction::setFileType(const QString& fileType)
{
    if (fileType == _fileType)
        return;

    _fileType = fileType;

    emit fileTypeChanged(_fileType);
}

QString FilePickerAction::getPlaceholderString() const
{
    return _filePathAction.getPlaceholderString();
}

void FilePickerAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == getPlaceholderString())
        return;

    _filePathAction.setPlaceHolderString(placeholderString);
}

QString FilePickerAction::getDirectoryName() const
{
    return QDir(getFilePath()).dirName();
}

bool FilePickerAction::isValid() const
{
    return !getFilePath().isEmpty() && QFileInfo(getFilePath()).exists();
}

QWidget* FilePickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(_filePathAction.createWidget(parent));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(_pickAction.createWidget(parent));

    widget->setLayout(layout);

    return widget;
}

void FilePickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    if (variantMap.contains("Value"))
        setFilePath(variantMap["Value"].toString());
}

QVariantMap FilePickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getFilePath() }
    });

    return variantMap;
}

}
