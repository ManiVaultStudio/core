#pragma once

#include "util/Serializable.h"
#include "util/Version.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"
#include "actions/IntegralAction.h"

#include "Application.h"

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

public: // Miscellaneous

    /**
     * Get version of the application (major and minor version number) with which the project is created
     * @return Pair of integers representing major and minor version number respectively
     */
    util::Version getVersion() const;

    /** Update the contributors (adds current user if not already added) */
    void updateContributors();

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
    const gui::StringsAction& getContributorsAction() const { return _contributorsAction; }
    const gui::ToggleAction& getCompressionEnabledAction() const { return _compressionEnabledAction; }
    const gui::IntegralAction& getCompressionLevelAction() const { return _compressionLevelAction; }

    gui::StringAction& getTitleAction() { return _titleAction; }
    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }
    gui::StringAction& getCommentsAction() { return _commentsAction; }
    gui::StringsAction& getContributorsAction() { return _contributorsAction; }
    gui::ToggleAction& getCompressionEnabledAction() { return _compressionEnabledAction; }
    gui::IntegralAction& getCompressionLevelAction() { return _compressionLevelAction; }

signals:

    /**
     * Signals that the project file path changed
     * @param filePath Location on disk where the project resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString                 _filePath;                      /** Location on disk where the project resides */
    util::Version           _version;                       /** Version of the application with which the project is created */
    gui::StringAction       _titleAction;                   /** Workspace title action */
    gui::StringAction       _descriptionAction;             /** Description action */
    gui::StringsAction      _tagsAction;                    /** Tags action */
    gui::StringAction       _commentsAction;                /** Comments action */
    gui::StringsAction      _contributorsAction;            /** Contributors action */
    gui::ToggleAction       _compressionEnabledAction;      /** Action to enable/disable project file compression */
    gui::IntegralAction     _compressionLevelAction;        /** Action to control the amount of project file compression */

protected:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};

}
