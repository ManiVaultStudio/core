#pragma once

#include "WidgetAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include <QLineEdit>
#include <QString>
#include <QFileSystemModel>
#include <QCompleter>

class QWidget;

namespace hdps::gui {

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
     */
    Q_INVOKABLE FilePickerAction(QObject* parent, const QString& title, const QString& filePath = QString());
    
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
     * Get the name filters
     * @return Name filters
     */
    QStringList getNameFilters() const;

    /**
     * Set the name filters to \p nameFilters
     * @param filePath Name filters
     */
    void setNameFilters(const QStringList& nameFilters);

    /**
     * Get the default suffix
     * @return Default suffix
     */
    QString getDefaultSuffix() const;

    /**
     * Set the default suffix to \p defaultSuffix
     * @param defaultSuffix Default suffix
     */
    void setDefaultSuffix(const QString& defaultSuffix);

    /**
     * Get the file type
     * @return File type
     */
    QString getFileType() const;

    /**
     * Set file type to \p fileType
     * @param fileType File type
     */
    void setFileType(const QString& fileType);

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

public: // Serialization

    /**
     * Load file picker action from variant
     * @param Variant representation of the file picker action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save file picker action to variant
     * @return Variant representation of the file picker action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    StringAction& getFilePathAction() { return _filePathAction; }
    TriggerAction& getPickAction() { return _pickAction; }

signals:

    /**
     * Signals that the file path changed to \p filePath
     * @param filePath New file path
     */
    void filePathChanged(const QString& filePath);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

    /**
     * Signals that the file type has changed to \p fileType
     * @param fileType File type
     */
    void fileTypeChanged(const QString& fileType);

private:
    QFileSystemModel    _dirModel;          /** Directory model */
    QCompleter          _completer;         /** Completer */
    StringAction        _filePathAction;    /** File path action */
    TriggerAction       _pickAction;        /** Pick file action */
    QStringList         _nameFilters;       /** File type filters */
    QString             _defaultSuffix;     /** Default suffix */
    QString             _fileType;          /** File type (e.g. image and project)*/
};

}

Q_DECLARE_METATYPE(hdps::gui::FilePickerAction)

inline const auto filePickerActionMetaTypeId = qRegisterMetaType<hdps::gui::FilePickerAction*>("hdps::gui::FilePickerAction");
