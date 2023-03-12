#pragma once

#include "util/Serializable.h"
#include "util/Version.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"
#include "actions/IntegralAction.h"
#include "actions/ProjectSplashScreenAction.h"
#include "actions/VersionAction.h"

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

    /** Container action class for compression parameters */
    class CompressionAction final : public gui::WidgetAction {
    public:

        /**
         * Constructs from \p parent object
         * @param parent Pointer to parent object
         */
        CompressionAction(QObject* parent = nullptr);

    public: // Serialization

        /**
         * Load compression action from variant
         * @param Variant representation of the compression action
         */
        void fromVariantMap(const QVariantMap& variantMap) override;

        /**
         * Save compression action to variant
         * @return Variant representation of the compression action
         */
        QVariantMap toVariantMap() const override;

    public: // Action getters

        gui::ToggleAction& getEnabledAction() { return _enabledAction; }
        gui::IntegralAction& getLevelAction() { return _levelAction; }

    private:
        gui::ToggleAction       _enabledAction;     /** Action to enable/disable project file compression */
        gui::IntegralAction     _levelAction;       /** Action to control the amount of project file compression */

    public:
        static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
        static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
    };

public:

    /**
    * Construct project with \p parent
    * @param parent Pointer to parent object
    */
    Project(QObject* parent = nullptr);

    /**
     * Construct project from project JSON \p filePath and \p parent 
     * @param filePath Path of the project file
     * @param preview Only extract basic info from the project JSON file for preview purposes
     * @param parent Pointer to parent object
     */
    Project(const QString& filePath, bool preview, QObject* parent = nullptr);

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
     * @param preview Only extract basic info from the project JSON file for preview purposes
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap, bool preview);

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

    const gui::VersionAction& getApplicationVersionAction() const { return _applicationVersionAction; }
    const gui::VersionAction& getProjectVersionAction() const { return _projectVersionAction; }
    const gui::ToggleAction& getReadOnlyAction() const { return _readOnlyAction; }
    const gui::StringAction& getTitleAction() const { return _titleAction; }
    const gui::StringAction& getDescriptionAction() const { return _descriptionAction; }
    const gui::StringsAction& getTagsAction() const { return _tagsAction; }
    const gui::StringAction& getCommentsAction() const { return _commentsAction; }
    const gui::StringsAction& getContributorsAction() const { return _contributorsAction; }
    const CompressionAction& getCompressionAction() const { return _compressionAction; }
    const gui::ProjectSplashScreenAction& getSplashScreenAction() const { return _splashScreenAction; }

    gui::VersionAction& getApplicationVersionAction() { return _applicationVersionAction; }
    gui::VersionAction& getProjectVersionAction() { return _projectVersionAction; }
    gui::ToggleAction& getReadOnlyAction() { return _readOnlyAction; }
    gui::StringAction& getTitleAction() { return _titleAction; }
    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }
    gui::StringAction& getCommentsAction() { return _commentsAction; }
    gui::StringsAction& getContributorsAction() { return _contributorsAction; }
    CompressionAction& getCompressionAction() { return _compressionAction; }
    gui::ProjectSplashScreenAction& getSplashScreenAction() { return _splashScreenAction; }

signals:

    /**
     * Signals that the project file path changed
     * @param filePath Location on disk where the project resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString                         _filePath;                  /** Location on disk where the project resides */
    util::Version                   _applicationVersion;        /** Version of the application with which the project is created */
    gui::VersionAction              _applicationVersionAction;  /** Action for storing the project version */
    gui::VersionAction              _projectVersionAction;      /** Action for storing the project version */
    gui::ToggleAction               _readOnlyAction;            /** Read-only action */
    gui::StringAction               _titleAction;               /** Title action */
    gui::StringAction               _descriptionAction;         /** Description action */
    gui::StringsAction              _tagsAction;                /** Tags action */
    gui::StringAction               _commentsAction;            /** Comments action */
    gui::StringsAction              _contributorsAction;        /** Contributors action */
    CompressionAction               _compressionAction;         /** Compression action */
    gui::ProjectSplashScreenAction  _splashScreenAction;        /** Action for configuring the project splash screen */

protected:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};

}
