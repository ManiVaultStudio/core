#include "FilePickerAction.h"

#include <Application.h>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

FilePickerAction::FilePickerAction(QObject* parent, const QString& title /*= ""*/, const QString& filePath /*= QString()*/, const QString& defaultFilePath /*= QString()*/) :
    WidgetAction(parent),
    _dirModel(),
    _completer(),
    _filePathAction(this, "File path"),
    _pickAction(this, "Pick")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(filePath, defaultFilePath);

    _completer.setModel(&_dirModel);

    // Show directory line edit action
    _filePathAction.getTrailingAction().setVisible(true);
    _filePathAction.setCompleter(&_completer);

    // Configure pick action
    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _pickAction.setToolTip("Click to choose a file");

    // Disable trailing action
    _filePathAction.getTrailingAction().setEnabled(false);

    // Update the trailing action of the string action to indicate whether the (typed) directory is valid or not
    const auto updateStatusAction = [this]() -> void {
        _filePathAction.getTrailingAction().setIcon(isValid() ? Application::getIconFont("FontAwesome").getIcon("check") : Application::getIconFont("FontAwesome").getIcon("exclamation"));
        _filePathAction.getTrailingAction().setToolTip(isValid() ? "File exists" : "File does not exist");
    };

    // Initial update of the status action
    updateStatusAction();

    // Update the line edit string when the directory string changes
    connect(&_filePathAction, &StringAction::stringChanged, this, updateStatusAction);

    // Open file dialog when pick action is triggered
    connect(&_pickAction, &TriggerAction::triggered, this, [this]() {
        const auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, tr("Open file"));

        if (!fileUrl.isValid())
            setFilePath(fileUrl.toString());
    });

    // Pass-through action signals
    connect(&_filePathAction, &StringAction::stringChanged, this, &FilePickerAction::directoryChanged);
    connect(&_filePathAction, &StringAction::defaultStringChanged, this, &FilePickerAction::defaultDirectoryChanged);
    connect(&_filePathAction, &StringAction::placeholderStringChanged, this, &FilePickerAction::placeholderStringChanged);
}

void FilePickerAction::initialize(const QString& filePath /*= QString()*/, const QString& defaultFilePath /*= QString()*/)
{
    setFilePath(filePath);
    setDefaultFilePath(defaultFilePath);
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

QString FilePickerAction::getDefaultFilePath() const
{
    return _filePathAction.getDefaultString();
}

void FilePickerAction::setDefaultFilePath(const QString& defaultFilePath)
{
    if (defaultFilePath == getDefaultFilePath())
        return;

    _filePathAction.setDefaultString(defaultFilePath);
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
    return !getFilePath().isEmpty() && QDir(getFilePath()).exists();
}

QWidget* FilePickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(_filePathAction.createWidget(parent));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(_pickAction.createWidget(parent));

    widget->setLayout(layout);

    return widget;
}

}
}
