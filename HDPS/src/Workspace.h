#pragma once

#include <util/Serializable.h>

#include <QObject>

namespace hdps {

/**
 * Workspace class
 *
 * TODO: Add description
 * 
 * @author Thomas Kroes
 */
class Workspace final : public QObject, public util::Serializable
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

signals:
    
    /**
     * Signals that the workspace file path changed
     * @param filePath Location on disk where the workspace resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString     _filePath;      /** Location on disk where the workspace resides */
};

}
