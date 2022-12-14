#pragma once

#include <util/Serializable.h>

namespace hdps {

/**
 * Project class
 *
 * TODO: Write description
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
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the project file path changed
     * @param filePath Location on disk where the project resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString     _filePath;      /** Location on disk where the project resides */
};

}
