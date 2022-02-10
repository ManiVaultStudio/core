#include "DirectoryPickerAction.h"

#include <Application.h>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

DirectoryPickerAction::DirectoryPickerAction(QObject* parent, const QString& title /*= ""*/, const QString& directory /*= QString()*/, const QString& defaultDirectory /*= QString()*/) :
    WidgetAction(parent),
    _dirModel(),
    _completer(),
    _directoryAction(this, "Type directory"),
    _pickAction(this, "Pick directory")
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(directory, defaultDirectory);

    _completer.setModel(&_dirModel);

    // Show directory line edit action
    _directoryAction.getTrailingAction().setVisible(true);
    _directoryAction.setCompleter(&_completer);

    // Configure pick action
    _pickAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _pickAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder"));
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
        const auto directory = QFileDialog::getExistingDirectory(nullptr, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!directory.isEmpty())
            setDirectory(directory);
    });

    // Pass-through action signals
    connect(&_directoryAction, &StringAction::stringChanged, this, &DirectoryPickerAction::directoryChanged);
    connect(&_directoryAction, &StringAction::defaultStringChanged, this, &DirectoryPickerAction::defaultDirectoryChanged);
    connect(&_directoryAction, &StringAction::placeholderStringChanged, this, &DirectoryPickerAction::placeholderStringChanged);
}

void DirectoryPickerAction::initialize(const QString& directory /*= QString()*/, const QString& defaultDirectory /*= QString()*/)
{
    setDirectory(directory);
    setDefaultDirectory(defaultDirectory);

    setResettable(isResettable());
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

    setResettable(isResettable());
}

QString DirectoryPickerAction::getDefaultDirectory() const
{
    return _directoryAction.getDefaultString();
}

void DirectoryPickerAction::setDefaultDirectory(const QString& defaultDirectory)
{
    if (defaultDirectory == getDefaultDirectory())
        return;

    _directoryAction.setDefaultString(defaultDirectory);

    setResettable(isResettable());
}

bool DirectoryPickerAction::isResettable() const
{
    return _directoryAction.isResettable();
}

void DirectoryPickerAction::reset()
{
    _directoryAction.reset();
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
    return QDir(getDirectory()).exists();
}

/*
DirectoryPickerAction::LineEditWidget::LineEditWidget(QWidget* parent, DirectoryPickerAction* directoryPickerAction) :
    QLineEdit(parent),
    _statusAction(this)
{
    setObjectName("LineEdit");
    addAction(&_statusAction, QLineEdit::TrailingPosition);

    // Update the line edit text from the string action
    const auto updateLineEdit = [this, directoryPickerAction]() {
        QSignalBlocker blocker(this);

        setText(directoryPickerAction->getDirectory());
    };

    // Update the place holder text in the line edit
    const auto updatePlaceHolderText = [this, directoryPickerAction]() -> void {
        setPlaceholderText(directoryPickerAction->getPlaceholderString());
    };

    

    // Update the line edit placeholder string when the action placeholder string changes
    connect(directoryPickerAction, &DirectoryPickerAction::placeholderStringChanged, this, updatePlaceHolderText);

    // Update the action string when the line edit text changes
    connect(this, &QLineEdit::textChanged, this, [this, directoryPickerAction](const QString& text) {
        directoryPickerAction->setDirectory(text);
    });

    // Perform initial updates
    updateLineEdit();
    updatePlaceHolderText();
    updateStatusAction();
}

DirectoryPickerAction::PushButtonWidget::PushButtonWidget(QWidget* parent, DirectoryPickerAction* directoryPickerAction) :
    QPushButton(parent)
{
    setObjectName("PushButton");
    setProperty("class", "square-button");
    

    
}
*/

QWidget* DirectoryPickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(_directoryAction.createWidget(parent));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(_pickAction.createWidget(parent));

    widget->setLayout(layout);

    return widget;
}

DirectoryPickerAction::Widget::Widget(QWidget* parent, DirectoryPickerAction* directoryPickerAction)
{

}

}
}
