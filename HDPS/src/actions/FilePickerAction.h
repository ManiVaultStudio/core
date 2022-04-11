#pragma once

#include "WidgetAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include <QLineEdit>
#include <QString>
#include <QFileSystemModel>
#include <QCompleter>

class QWidget;

namespace hdps {

namespace gui {

/**
 * File picker action class
 *
 * Action class for picking a file
 *
 * @author Thomas Kroes
 */
class FilePickerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        LineEdit    = 0x00001,      /** Widget includes a line edit */
        PushButton  = 0x00002,      /** There is a button to pick a directory */

        Default = LineEdit | PushButton
    };

protected:

    /**
     * Get widget representation of the file picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param filePath File path
     * @param defaultFilePath Default file path
     */
    FilePickerAction(QObject* parent, const QString& title = "", const QString& filePath = QString(), const QString& defaultFilePath = QString());

    /**
     * Initialize the directory picker action
     * @param filePath File path
     * @param defaultFilePath Default file path
     */
    void initialize(const QString& filePath = QString(), const QString& defaultFilePath = QString());

    /**
     * Get the current file path
     * @return Current file path
     */
    QString getFilePath() const;

    /**
     * Set the file path
     * @param filePath Current file path
     */
    void setFilePath(const QString& filePath);

    /**
     * Get the default file path
     * @return Default file path
     */
    QString getDefaultFilePath() const;

    /**
     * Set the default file path
     * @param defaultFilePath Default file path
     */
    void setDefaultFilePath(const QString& defaultFilePath);

    /**
     * Get placeholder text
     * @return Placeholder string
     */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderText Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /**
     * Get directory name
     * @return Directory name
     */
    QString getDirectoryName() const;

    /**
     * Get whether the directory is valid or not
     * @return Boolean indication whether the directory is valid or not
     */
    bool isValid() const;

signals:

    /**
     * Signals that the directory changed
     * @param directory Directory that changed
     */
    void directoryChanged(const QString& directory);

    /**
     * Signals that the default directory changed
     * @param defaultString Default directory that changed
     */
    void defaultDirectoryChanged(const QString& defaultDirectory);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

protected:
    QFileSystemModel    _dirModel;              /** Directory model */
    QCompleter          _completer;             /** Completer */
    StringAction        _filePathAction;        /** File path action */
    TriggerAction       _pickAction;            /** Pick file action */
};

}
}
