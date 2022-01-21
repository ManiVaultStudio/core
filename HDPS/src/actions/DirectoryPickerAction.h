#pragma once

#include "WidgetAction.h"

#include <QLineEdit>
#include <QDir>

class QWidget;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Directory picker action class
 *
 * Action class for picking a directory
 *
 * @author Thomas Kroes
 */
class DirectoryPickerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        LineEdit        = 0x00001,      /** Widget includes a line edit */
        PickPushButton  = 0x00002,      /** There is a button to pick a directory */

        Default = LineEdit | PickPushButton
    };

public:

    /** Line edit widget class for directory picker action */
    class LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param directoryPickerAction Pointer to directory picker action
         */
        LineEditWidget(QWidget* parent, DirectoryPickerAction* directoryPickerAction);

        friend class DirectoryPickerAction;
    };

protected:

    /**
     * Get widget representation of the directory picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param directory Directory
     * @param defaultDirectory Default directory
     */
    DirectoryPickerAction(QObject* parent, const QString& title = "", const QDir& directory = QDir(), const QDir& defaultDirectory = QDir());

    /**
     * Initialize the directory picker action
     * @param directory Directory
     * @param defaultDirectory Default directory
     */
    void initialize(const QDir& directory = QDir(), const QDir& defaultDirectory = QDir());

    /**
     * Get the current directory
     * @return Current directory
     */
    QDir getDirectory() const;

    /**
     * Set the current directory
     * @param directory Current directory
     */
    void setDirectory(const QDir& directory);

    /**
     * Get the default directory
     * @return Default directory
     */
    QDir getDefaultDirectory() const;

    /**
     * Set the default directory
     * @param defaultDirectory Default directory
     */
    void setDefaultDirectory(const QDir& defaultDirectory);

    /** Determines whether the current directory can be reset to its default directory */
    bool isResettable() const override;

    /** Reset the current directory to the default directory */
    void reset() override;

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

signals:

    /**
     * Signals that the directory changed
     * @param directory Directory that changed
     */
    void directoryChanged(const QDir& directory);

    /**
     * Signals that the default directory changed
     * @param defaultString Default directory that changed
     */
    void defaultDirectoryChanged(const QDir& defaultDirectory);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

protected:
    QDir        _directory;             /** Current directory */
    QDir        _defaultDirectory;      /** Default directory */
    QString     _placeholderString;     /** Place holder string */
};

}
}
