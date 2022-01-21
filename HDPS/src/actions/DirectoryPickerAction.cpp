#include "DirectoryPickerAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

DirectoryPickerAction::DirectoryPickerAction(QObject* parent, const QString& title /*= ""*/, const QDir& directory /*= QDir()*/, const QDir& defaultDirectory /*= QDir()*/) :
    WidgetAction(parent)
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(directory, defaultDirectory);
}

void DirectoryPickerAction::initialize(const QDir& directory /*= QDir()*/, const QDir& defaultDirectory /*= QDir()*/)
{
    setDirectory(directory);
    setDefaultDirectory(defaultDirectory);

    setResettable(isResettable());
}

QDir DirectoryPickerAction::getDirectory() const
{
    return _directory;
}

void DirectoryPickerAction::setDirectory(const QDir& directory)
{
    if (directory == _directory)
        return;

    _directory = directory;

    emit directoryChanged(_directory);

    setResettable(isResettable());
}

QDir DirectoryPickerAction::getDefaultDirectory() const
{
    return _defaultDirectory;
}

void DirectoryPickerAction::setDefaultDirectory(const QDir& defaultDirectory)
{
    if (defaultDirectory == _defaultDirectory)
        return;

    _defaultDirectory = defaultDirectory;

    emit defaultDirectoryChanged(_defaultDirectory);

    setResettable(isResettable());
}

bool DirectoryPickerAction::isResettable() const
{
    return _directory != _defaultDirectory;
}

void DirectoryPickerAction::reset()
{
    setDirectory(_defaultDirectory);
}

QString DirectoryPickerAction::getPlaceholderString() const
{
    return _placeholderString;
}

void DirectoryPickerAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == _placeholderString)
        return;

    _placeholderString = placeholderString;

    emit placeholderStringChanged(_placeholderString);
}

DirectoryPickerAction::LineEditWidget::LineEditWidget(QWidget* parent, DirectoryPickerAction* directoryPickerAction) :
    QLineEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);

    // Update the line edit text from the string action
    const auto updateLineEdit = [this, directoryPickerAction]() {
        QSignalBlocker blocker(this);

        setText(directoryPickerAction->getDirectory().dirName());
    };

    // Update the place holder text in the line edit
    const auto updatePlaceHolderText = [this, directoryPickerAction]() -> void {
        setPlaceholderText(directoryPickerAction->getPlaceholderString());
    };

    // Update the line edit string when the action string changes
    connect(directoryPickerAction, &DirectoryPickerAction::directoryChanged, this, updateLineEdit);

    // Update the line edit placeholder string when the action placeholder string changes
    connect(directoryPickerAction, &DirectoryPickerAction::placeholderStringChanged, this, updatePlaceHolderText);

    // Update the action string when the line edit text changes
    connect(this, &QLineEdit::textChanged, this, [this, directoryPickerAction](const QString& text) {
        directoryPickerAction->setDirectory(text);
    });

    // Perform initial updates
    updateLineEdit();
    updatePlaceHolderText();
}

QWidget* DirectoryPickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new DirectoryPickerAction::LineEditWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
}
