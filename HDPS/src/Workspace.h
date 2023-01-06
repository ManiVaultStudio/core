#pragma once

#include "util/Serializable.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"

namespace hdps {

/**
 * Workspace class
 *
 * TODO: Write description.
 *
 * @author Thomas Kroes
 */
class Workspace final : public QObject, public hdps::util::Serializable
{
    Q_OBJECT

public:

    /**
     * Construct workspace with \p parent
     * @param parent Pointer to parent object
     */
    Workspace(QObject* parent = nullptr);

    /**
     * Construct workspace with \p parent and load from \p filePath
     * @param filePath Path of the workspace file
     * @param parent Pointer to parent object
     */
    Workspace(const QString& filePath, QObject* parent = nullptr);

    /**
     * Get workspace file path
     * @return Location on disk where the workspace resides
     */
    QString getFilePath() const;

protected:

    /**
     * Set workspace file path
     * @param filePath Location on disk where the workspace resides
     */
    void setFilePath(const QString& filePath);

public:

    /**
     * Get preview image
     * @param filePath Path of the work space file
     * @return Preview image
     */
    static QImage getPreviewImage(const QString& filePath);

public: // Serialization

    /**
     * Load workspace from variant
     * @param Variant representation of the workspace
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save workspace to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /** Startup initialization */
    void initialize();

public: // Action getters

    const gui::StringAction& getTitleAction() const { return _titleAction; }
    const gui::StringAction& getDescriptionAction() const { return _descriptionAction; }
    const gui::StringsAction& getTagsAction() const { return _tagsAction; }
    const gui::StringAction& getCommentsAction() const { return _commentsAction; }

    gui::StringAction& getTitleAction() { return _titleAction; }
    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }
    gui::StringAction& getCommentsAction() { return _commentsAction; }

signals:

    /**
     * Signals that the workspace file path changed
     * @param filePath Location on disk where the workspace resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString             _filePath;              /** Location on disk where the workspace resides */
    gui::StringAction   _titleAction;           /** Workspace title action */
    gui::StringAction   _descriptionAction;     /** Workspace description action */
    gui::StringsAction  _tagsAction;            /** Workspace tags action */
    gui::StringAction   _commentsAction;        /** Workspace comments action */

    friend class AbstractWorkspaceManager;
};

}
