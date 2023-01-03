#pragma once

#include "util/Serializable.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"

namespace hdps {

/**
 * Workspace class
 *
 * TODO: Write description
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
     * Get workspace file path
     * @return Location on disk where the workspace resides
     */
    QString getFilePath() const;

    /**
     * Set workspace file path
     * @param filePath Location on disk where the workspace resides
     */
    void setFilePath(const QString& filePath);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }

signals:

    /**
     * Signals that the workspace file path changed
     * @param filePath Location on disk where the workspace resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString             _filePath;              /** Location on disk where the workspace resides */
    gui::StringAction   _descriptionAction;     /** Work description action */
    gui::StringsAction  _tagsAction;            /** Workspace tags action */
};

}
