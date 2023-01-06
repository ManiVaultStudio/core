#pragma once

#include "util/Serializable.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"

namespace hdps {

/**
 * Project class
 *
 * TODO: Write description.
 *
 * @author Thomas Kroes
 */
class Project final : public QObject, public hdps::util::Serializable
{
    Q_OBJECT

public:

    /**
    * Construct project with \p parent
    * @param parent Pointer to parent object
    */
    Project(QObject* parent = nullptr);

    /**
     * Construct project with \p parent and load from \p filePath
     * @param filePath Path of the project file
     * @param parent Pointer to parent object
     */
    Project(const QString& filePath, QObject* parent = nullptr);

    /**
     * Get project file path
     * @return Location on disk where the project resides
     */
    QString getFilePath() const;

    /**
     * Set project file path
     * @param filePath Location on disk where the project resides
     */
    void setFilePath(const QString& filePath);

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
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
     * Signals that the project file path changed
     * @param filePath Location on disk where the project resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString             _filePath;              /** Location on disk where the project resides */
    gui::StringAction   _titleAction;           /** Workspace title action */
    gui::StringAction   _descriptionAction;     /** Work description action */
    gui::StringsAction  _tagsAction;            /** Workspace tags action */
    gui::StringAction   _commentsAction;        /** Workspace comments action */
};

}
