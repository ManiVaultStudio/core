// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FilePickerAction.h"
#include "Application.h"

#include "actions/HorizontalGroupAction.h"

#include "widgets/FileDialog.h"

#include <QDir>
#include <QStandardPaths>

using namespace mv::util;

namespace mv::gui {

FilePickerAction::FilePickerAction(QObject* parent, const QString& title, const QString& filePath /*= QString()*/, bool populateCompleter /*= true*/) :
    WidgetAction(parent, title),
    _dirModel(nullptr),
    _completer(nullptr),
    _filePathAction(this, "File path"),
    _pickAction(this, "Pick"),
    _fileType("File"),
    _useNativeDialog(true)
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

    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIconByName("folder-open");
    
    const auto updatePickActionToolTip = [this]() -> void {
        _pickAction.setToolTip(QString("Click to browse for %1").arg(getFileType().toLower()));
    };

    updatePickActionToolTip();

    connect(this, &FilePickerAction::fileTypeChanged, this, updatePickActionToolTip);

    _filePathAction.getTrailingAction().setEnabled(false);

    const auto updateStatusAction = [this]() -> void {
        _filePathAction.getTrailingAction().setIcon(isValid() ? StyledIcon("check") : StyledIcon("exclamation"));
        _filePathAction.getTrailingAction().setToolTip(isValid() ? "File exists" : "File does not exist");
    };

    updateStatusAction();

    connect(&_filePathAction, &StringAction::stringChanged, this, updateStatusAction);

    connect(&_pickAction, &TriggerAction::triggered, this, [this]() {
        auto* fileDialog = new FileOpenDialog(nullptr);

        fileDialog->setWindowTitle(QString("Open %1").arg(getFileType()));
        fileDialog->setNameFilters(getNameFilters());
        fileDialog->setDefaultSuffix(getDefaultSuffix());
        fileDialog->setDirectory(Application::current()->getSetting(getSettingsPrefix(), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
        fileDialog->setOption(QFileDialog::DontUseNativeDialog, !_useNativeDialog);

		connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            const auto filePath = fileDialog->selectedFiles().first();

            setFilePath(filePath);

            if (!getSettingsPrefix().isEmpty())
                setFilePath(QFileInfo(filePath).absoluteFilePath());

            QDir applicationDir;

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
            applicationDir = QCoreApplication::applicationDirPath();
#endif

#ifdef Q_OS_MAC
            applicationDir = QDir(QCoreApplication::applicationDirPath() + "/../..").canonicalPath();
#endif

            const auto canonicalAppDir      = applicationDir.canonicalPath();
            const auto canonicalFilePath    = QFileInfo(filePath).canonicalFilePath();

            if (canonicalFilePath.startsWith(canonicalAppDir + QDir::separator()))
                _applicationRelativeFilePath = QDir(filePath).relativeFilePath(applicationDir.path());
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

QString FilePickerAction::getApplicationRelativeFilePath() const
{
    return _applicationRelativeFilePath;
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

void FilePickerAction::setUseNativeFileDialog(bool useNativeDialog)
{
    _useNativeDialog = useNativeDialog;
}

bool FilePickerAction::getUseNativeFileDialog() const
{
    return _useNativeDialog;
}

bool FilePickerAction::isValid() const
{
    return !getFilePath().isEmpty() && QFileInfo(getFilePath()).exists();
}

QString FilePickerAction::getApplicationDirectory() const
{
    return _applicationRelativeFilePath;
}

QWidget* FilePickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto groupAction = new HorizontalGroupAction(this, "Group");

    if (widgetFlags & WidgetFlag::LineEdit)
        groupAction->addAction(&_filePathAction);

    if (widgetFlags & WidgetFlag::PushButton)
        groupAction->addAction(&_pickAction);

    return groupAction->createWidget(parent);
}

void FilePickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    if (variantMap.contains("Value"))
        setFilePath(variantMap["Value"].toString());

	if (variantMap.contains("RelativeFilePath"))
        _applicationRelativeFilePath = variantMap["RelativeFilePath"].toString();
}

QVariantMap FilePickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getFilePath() },
        { "RelativeFilePath", _applicationRelativeFilePath }
    });

    return variantMap;
}

}
