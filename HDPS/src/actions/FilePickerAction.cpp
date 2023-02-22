#include "FilePickerAction.h"

#include <Application.h>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QStandardPaths>

using namespace hdps::util;

namespace hdps::gui {

FilePickerAction::FilePickerAction(QObject* parent, const QString& title /*= ""*/, const QString& filePath /*= QString()*/) :
    WidgetAction(parent),
    _dirModel(),
    _completer(),
    _filePathAction(this, "File path"),
    _pickAction(this, "Pick"),
    _nameFilters(),
    _defaultSuffix(),
    _fileType("File")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);

    _completer.setModel(&_dirModel);

    _filePathAction.getTrailingAction().setVisible(true);
    _filePathAction.setCompleter(&_completer);

    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    
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
        QFileDialog fileDialog;

        fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog.setWindowTitle(QString("Open %1").arg(getFileType()));
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setNameFilters(getNameFilters());
        fileDialog.setDefaultSuffix(getDefaultSuffix());
        fileDialog.setDirectory(Application::current()->getSetting(getSettingsPrefix(), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
        fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

        if (fileDialog.exec() == 0)
            return;

        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        const auto filePath = fileDialog.selectedFiles().first();

        setFilePath(filePath);

        if (!getSettingsPrefix().isEmpty())
            Application::current()->setSetting(getSettingsPrefix(), QFileInfo(filePath).absolutePath());
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
    layout->setSpacing(3);

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

    //variantMapMustContain(variantMap, "Value");

    //setFilePath(variantMap["Value"].toString());
}

QVariantMap FilePickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    //variantMap.insert({
    //    { "Value", QVariant::fromValue(getFilePath()) }
    //});

    return variantMap;
}

}
